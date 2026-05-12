#include "mainmenuwidget.h"
#include "ui_mainmenuwidget.h"
#include "clientapi.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QTableWidgetItem>

MainMenuWidget::MainMenuWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainMenuWidget)
{
    ui->setupUi(this);

    // Заполнение матрицы инцидентности по умолчанию
    ui->incidenceTable->setRowCount(3);
    ui->incidenceTable->setColumnCount(3);
    ui->incidenceTable->setItem(0, 0, new QTableWidgetItem("1"));
    ui->incidenceTable->setItem(0, 1, new QTableWidgetItem("1"));
    ui->incidenceTable->setItem(0, 2, new QTableWidgetItem("0"));
    ui->incidenceTable->setItem(1, 0, new QTableWidgetItem("1"));
    ui->incidenceTable->setItem(1, 1, new QTableWidgetItem("0"));
    ui->incidenceTable->setItem(1, 2, new QTableWidgetItem("1"));
    ui->incidenceTable->setItem(2, 0, new QTableWidgetItem("0"));
    ui->incidenceTable->setItem(2, 1, new QTableWidgetItem("1"));
    ui->incidenceTable->setItem(2, 2, new QTableWidgetItem("1"));

    connect(ui->task1Btn, &QPushButton::clicked, this, &MainMenuWidget::onTask1Clicked);
    connect(ui->task2Btn, &QPushButton::clicked, this, &MainMenuWidget::onTask2Clicked);
    connect(ui->task3Btn, &QPushButton::clicked, this, &MainMenuWidget::onTask3Clicked);
    connect(ui->task4Btn, &QPushButton::clicked, this, &MainMenuWidget::onTask4Clicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainMenuWidget::onLogoutClick);
    connect(&ClientAPI::instance(), &ClientAPI::responseReceived, this, &MainMenuWidget::onResponseReceived);

    ClientAPI::instance().connectToServer("127.0.0.1", 12345);
}

void MainMenuWidget::onTask1Clicked()
{
    int vertices = ui->verticesSpin->value();
    int edges = ui->edgesSpin->value();

    QJsonArray incidenceMatrix;
    for (int i = 0; i < vertices; i++) {
        QJsonArray row;
        for (int j = 0; j < edges; j++) {
            QTableWidgetItem *item = ui->incidenceTable->item(i, j);
            int value = item ? item->text().toInt() : 0;
            row.append(value);
        }
        incidenceMatrix.append(row);
    }

    QJsonObject request;
    request["action"] = "task1";
    request["incidenceMatrix"] = incidenceMatrix;
    request["vertices"] = vertices;
    request["edges"] = edges;

    ClientAPI::instance().sendRequest(request);
    ui->outputEdit->append("Задача 1: Отправлен запрос на вычисление степеней вершин...");
}

void MainMenuWidget::onTask2Clicked()
{
    int currentDist = ui->currentDistSpin->value();
    int edgeWeight = ui->edgeWeightSpin->value();

    QJsonObject request;
    request["action"] = "task2";
    request["currentDistance"] = currentDist;
    request["edgeWeight"] = edgeWeight;

    ClientAPI::instance().sendRequest(request);
    ui->outputEdit->append(QString("Задача 2: Отправлен запрос на релаксацию ребра..."));
}

void MainMenuWidget::onTask3Clicked()
{
    int vertices = ui->bVerticesSpin->value();
    QJsonArray edges = QJsonDocument::fromJson(ui->bEdgesInput->toPlainText().toUtf8()).array();

    QJsonObject request;
    request["action"] = "task3";
    request["edges"] = edges;
    request["vertices"] = vertices;

    ClientAPI::instance().sendRequest(request);
    ui->outputEdit->append("Задача 3: Отправлен запрос на разбиение двудольного графа...");
}

void MainMenuWidget::onTask4Clicked()
{
    int vertices = ui->pathVerticesSpin->value();
    int start = ui->startVertexSpin->value();
    int end = ui->endVertexSpin->value();
    QJsonArray edges = QJsonDocument::fromJson(ui->pathEdgesInput->toPlainText().toUtf8()).array();

    QJsonObject request;
    request["action"] = "task4";
    request["edges"] = edges;
    request["vertices"] = vertices;
    request["start"] = start;
    request["end"] = end;

    ClientAPI::instance().sendRequest(request);
    ui->outputEdit->append(QString("Задача 4: Отправлен запрос на поиск пути между %1 и %2...").arg(start).arg(end));
}

void MainMenuWidget::onResponseReceived(const QJsonObject &response)
{
    QString action = response["action"].toString();

    if (action == "task1_result") {
        QJsonArray degrees = response["degrees"].toArray();
        QString degStr;
        for (int i = 0; i < degrees.size(); i++) {
            degStr += QString("Вершина %1: степень %2\n").arg(i).arg(degrees[i].toInt());
        }
        ui->outputEdit->append("Результат задачи 1:\n" + degStr);
    }
    else if (action == "task2_result") {
        ui->outputEdit->append(QString("Результат задачи 2:\n%1").arg(response["message"].toString()));
    }
    else if (action == "task3_result") {
        if (response["is_bipartite"].toBool()) {
            QJsonArray partA = response["partA"].toArray();
            QJsonArray partB = response["partB"].toArray();
            QString aStr, bStr;
            for (int i = 0; i < partA.size(); i++) {
                aStr += QString::number(partA[i].toInt()) + " ";
            }
            for (int i = 0; i < partB.size(); i++) {
                bStr += QString::number(partB[i].toInt()) + " ";
            }
            ui->outputEdit->append(QString("Результат задачи 3:\nДоля A: [%1]\nДоля B: [%2]").arg(aStr).arg(bStr));
        } else {
            ui->outputEdit->append("Результат задачи 3:\nГраф НЕ является двудольным!");
        }
    }
    else if (action == "task4_result") {
        ui->outputEdit->append("Результат задачи 4:\n" + response["message"].toString());
    }
    else if (action == "login_response") {
        if (response["status"] == "success") {
            emit loginSuccess(response["username"].toString(), response["role"].toString());
        } else {
            ui->outputEdit->append("Ошибка входа: " + response["message"].toString());
        }
    }
}

void MainMenuWidget::setUserInfo(const QString &username, const QString &role)
{
    ui->userInfoLabel->setText(QString("User: %1 (%2)").arg(username).arg(role));
    if (role != "admin") {
        ui->tabWidget->setEnabled(false);
        ui->outputEdit->append("Доступ только для чтения. Войдите как admin для вычислений.");
    } else {
        ui->outputEdit->append("Admin mode - все задачи доступны");
    }
}

void MainMenuWidget::onLogoutClick()
{
    ClientAPI::instance().disconnectFromServer();
    QMessageBox::information(this, "Logout", "Logged out");
    emit logoutRequested();
}

MainMenuWidget::~MainMenuWidget()
{
    delete ui;
}
