#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // Подключение сигналов сокета
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onServerResponse);
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::onConnected);

    // Подключение кнопок
    connect(ui->btnSolve1, &QPushButton::clicked, this, &MainWindow::onBtnSolve1Clicked);
    connect(ui->btnSolve2, &QPushButton::clicked, this, &MainWindow::onBtnSolve2Clicked);
    connect(ui->btnSolve3, &QPushButton::clicked, this, &MainWindow::onBtnSolve3Clicked);

    ui->statusLabel->setText("Статус: готов (сервер: localhost:33333)");
}

MainWindow::~MainWindow()
{
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        tcpSocket->disconnectFromHost();
    }
    delete ui;
}

void MainWindow::sendToServer(const QString &type, const QString &data)
{
    // Подключаемся к серверу
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        tcpSocket->disconnectFromHost();
        tcpSocket->waitForDisconnected(1000);
    }

    tcpSocket->connectToHost(QHostAddress::LocalHost, 33333);

    if (!tcpSocket->waitForConnected(3000)) {
        ui->statusLabel->setText("Статус: ошибка - сервер не запущен!");
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось подключиться к серверу!\n"
                              "Убедитесь, что echoServer запущен на порту 33333.");
        return;
    }

    // Создаём JSON запрос
    QJsonObject request;
    request["type"] = type;
    request["data"] = data;

    QJsonDocument doc(request);
    tcpSocket->write(doc.toJson());
    tcpSocket->flush();

    qDebug() << "Request sent:" << type;
}

void MainWindow::onConnected()
{
    qDebug() << "Connected to server";
}

void MainWindow::onBtnSolve1Clicked()
{
    QString input = ui->inputGraph1->toPlainText().trimmed();
    if (input.isEmpty()) {
        ui->output1->setText("Ошибка: введите матрицу смежности");
        return;
    }

    currentTask = "bipartite";
    ui->output1->setText("Отправка запроса на сервер...");
    ui->statusLabel->setText("Статус: отправка запроса (двудольный граф)...");
    sendToServer("bipartite", input);
}

void MainWindow::onBtnSolve2Clicked()
{
    QString input = ui->inputGraph2->toPlainText().trimmed();
    if (input.isEmpty()) {
        ui->output2->setText("Ошибка: введите матрицу инцидентности");
        return;
    }

    currentTask = "degrees";
    ui->output2->setText("Отправка запроса на сервер...");
    ui->statusLabel->setText("Статус: отправка запроса (степени вершин)...");
    sendToServer("degrees", input);
}

void MainWindow::onBtnSolve3Clicked()
{
    QString distStr = ui->distInput->text().trimmed();
    QString edgeStr = ui->edgeInput->text().trimmed();

    if (distStr.isEmpty() || edgeStr.isEmpty()) {
        ui->output3->setText("Ошибка: введите расстояния и ребро");
        return;
    }

    currentTask = "relaxation";
    ui->output3->setText("Отправка запроса на сервер...");
    ui->statusLabel->setText("Статус: отправка запроса (релаксация)...");
    sendToServer("relaxation", distStr + "|" + edgeStr);
}

void MainWindow::onServerResponse()
{
    QByteArray data = tcpSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isObject()) {
        QJsonObject response = doc.object();
        QString result = response["result"].toString();
        bool success = response["success"].toBool();

        if (currentTask == "bipartite") {
            ui->output1->setText(result);
            ui->statusLabel->setText(success ? "Статус: задача 1 выполнена" : "Статус: ошибка в задаче 1");
        }
        else if (currentTask == "degrees") {
            ui->output2->setText(result);
            ui->statusLabel->setText(success ? "Статус: задача 2 выполнена" : "Статус: ошибка в задаче 2");
        }
        else if (currentTask == "relaxation") {
            ui->output3->setText(result);
            ui->statusLabel->setText(success ? "Статус: задача 3 выполнена" : "Статус: ошибка в задаче 3");
        }

        qDebug() << "Response received, success:" << success;
    } else {
        ui->statusLabel->setText("Статус: неверный ответ от сервера");
    }

    // Отключаемся после получения ответа
    tcpSocket->disconnectFromHost();
}
