#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(nullptr)  // Инициализируем nullptr
    , currentTask("")
{
    ui->setupUi(this);

    // Создаём сокет ТОЛЬКО после setupUi
    tcpSocket = new QTcpSocket(this);

    // Проверяем, создался ли UI
    if (!ui) {
        qDebug() << "UI is null!";
        return;
    }

    // Подключение сигналов сокета
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::onServerResponse);
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        qDebug() << "Socket error:" << error;
        ui->statusLabel->setText("❌ Ошибка: " + tcpSocket->errorString());
    });

    // Подключение кнопок (с проверкой существования)
    if (ui->btnSolve1) connect(ui->btnSolve1, &QPushButton::clicked, this, &MainWindow::onBtnSolve1Clicked);
    if (ui->btnSolve2) connect(ui->btnSolve2, &QPushButton::clicked, this, &MainWindow::onBtnSolve2Clicked);
    if (ui->btnSolve3) connect(ui->btnSolve3, &QPushButton::clicked, this, &MainWindow::onBtnSolve3Clicked);

    if (ui->statusLabel) {
        ui->statusLabel->setText("✅ Статус: готов (сервер: localhost:12345)");
    }

    qDebug() << "MainWindow constructed successfully";
}

MainWindow::~MainWindow()
{
    if (tcpSocket && tcpSocket->state() == QTcpSocket::ConnectedState) {
        tcpSocket->disconnectFromHost();
    }
    delete ui;
}

void MainWindow::sendToServer(const QString &type, const QString &data)
{
    if (!tcpSocket) {
        QMessageBox::critical(this, "Ошибка", "Сокет не инициализирован!");
        return;
    }

    // Подключаемся к серверу
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        tcpSocket->disconnectFromHost();
        tcpSocket->waitForDisconnected(1000);
    }

    qDebug() << "Connecting to server...";
    tcpSocket->connectToHost(QHostAddress::LocalHost, 12345);

    if (!tcpSocket->waitForConnected(3000)) {
        QString errorMsg = "Не удалось подключиться к серверу!\n"
                           "Убедитесь, что echoServer запущен на порту 12345.\n\n"
                           "Ошибка: " + tcpSocket->errorString();

        ui->statusLabel->setText("❌ Статус: сервер не запущен!");
        QMessageBox::critical(this, "Ошибка", errorMsg);

        // Выводим результат в соответствующее поле
        if (type == "bipartite" && ui->output1) ui->output1->setText("❌ Ошибка: сервер не запущен");
        else if (type == "degrees" && ui->output2) ui->output2->setText("❌ Ошибка: сервер не запущен");
        else if (type == "relaxation" && ui->output3) ui->output3->setText("❌ Ошибка: сервер не запущен");
        return;
    }

    qDebug() << "Connected, sending request...";

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
    qDebug() << "Connected to server successfully";
    ui->statusLabel->setText("✅ Статус: подключено к серверу");
}

void MainWindow::onBtnSolve1Clicked()
{
    if (!ui->inputGraph1) return;

    QString input = ui->inputGraph1->toPlainText().trimmed();
    if (input.isEmpty()) {
        if (ui->output1) ui->output1->setText("❌ Ошибка: введите матрицу смежности");
        return;
    }

    currentTask = "bipartite";
    if (ui->output1) ui->output1->setText("⏳ Отправка запроса на сервер...");
    if (ui->statusLabel) ui->statusLabel->setText("Статус: отправка запроса (двудольный граф)...");
    sendToServer("bipartite", input);
}

void MainWindow::onBtnSolve2Clicked()
{
    if (!ui->inputGraph2) return;

    QString input = ui->inputGraph2->toPlainText().trimmed();
    if (input.isEmpty()) {
        if (ui->output2) ui->output2->setText("❌ Ошибка: введите матрицу инцидентности");
        return;
    }

    currentTask = "degrees";
    if (ui->output2) ui->output2->setText("⏳ Отправка запроса на сервер...");
    if (ui->statusLabel) ui->statusLabel->setText("Статус: отправка запроса (степени вершин)...");
    sendToServer("degrees", input);
}

void MainWindow::onBtnSolve3Clicked()
{
    if (!ui->distInput || !ui->edgeInput) return;

    QString distStr = ui->distInput->text().trimmed();
    QString edgeStr = ui->edgeInput->text().trimmed();

    if (distStr.isEmpty() || edgeStr.isEmpty()) {
        if (ui->output3) ui->output3->setText("❌ Ошибка: введите расстояния и ребро");
        return;
    }

    currentTask = "relaxation";
    if (ui->output3) ui->output3->setText("⏳ Отправка запроса на сервер...");
    if (ui->statusLabel) ui->statusLabel->setText("Статус: отправка запроса (релаксация)...");
    sendToServer("relaxation", distStr + "|" + edgeStr);
}

void MainWindow::onServerResponse()
{
    if (!tcpSocket) return;

    QByteArray data = tcpSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    qDebug() << "Response received:" << data;

    if (doc.isObject()) {
        QJsonObject response = doc.object();
        QString result = response["result"].toString();
        bool success = response["success"].toBool();

        if (currentTask == "bipartite") {
            if (ui->output1) ui->output1->setText(result);
            if (ui->statusLabel) ui->statusLabel->setText(success ? "✅ Статус: задача 1 выполнена" : "❌ Статус: ошибка в задаче 1");
        }
        else if (currentTask == "degrees") {
            if (ui->output2) ui->output2->setText(result);
            if (ui->statusLabel) ui->statusLabel->setText(success ? "✅ Статус: задача 2 выполнена" : "❌ Статус: ошибка в задаче 2");
        }
        else if (currentTask == "relaxation") {
            if (ui->output3) ui->output3->setText(result);
            if (ui->statusLabel) ui->statusLabel->setText(success ? "✅ Статус: задача 3 выполнена" : "❌ Статус: ошибка в задаче 3");
        }

        qDebug() << "Response processed, success:" << success;
    } else {
        if (ui->statusLabel) ui->statusLabel->setText("❌ Статус: неверный ответ от сервера");
        qDebug() << "Invalid JSON response";
    }

    // Отключаемся после получения ответа
    if (tcpSocket) {
        tcpSocket->disconnectFromHost();
    }
}
