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

    // Setup matrix table
    ui->matrixTable->setRowCount(3);
    ui->matrixTable->setColumnCount(3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ui->matrixTable->setItem(i, j, new QTableWidgetItem(i == j ? "1" : "0"));
        }
    }

    connect(ui->matrixBtn, &QPushButton::clicked, this, &MainMenuWidget::onMatrixClick);
    connect(ui->relaxationBtn, &QPushButton::clicked, this, &MainMenuWidget::onRelaxationClick);
    connect(ui->bipartiteBtn, &QPushButton::clicked, this, &MainMenuWidget::onBipartiteClick);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainMenuWidget::onLogoutClick);
    connect(&ClientAPI::instance(), &ClientAPI::responseReceived, this, &MainMenuWidget::onResponseReceived);
}

void MainMenuWidget::setUserInfo(const QString &username, const QString &role)
{
    ui->userInfoLabel->setText(QString("User: %1 (%2)").arg(username).arg(role));

    if (role != "admin") {
        ui->tabWidget->setEnabled(false);
        ui->outputEdit->append("Regular user mode - admin functions disabled");
    } else {
        ui->outputEdit->append("Admin mode - full access");
    }
}

void MainMenuWidget::onMatrixClick()
{
    QJsonArray matrix;
    for (int i = 0; i < 3; i++) {
        QJsonArray row;
        for (int j = 0; j < 3; j++) {
            row.append(ui->matrixTable->item(i, j)->text().toInt());
        }
        matrix.append(row);
    }

    QJsonObject req;
    req["action"] = "matrix_identity";
    req["matrix"] = matrix;
    req["size"] = 3;
    ClientAPI::instance().sendRequest(req);
    ui->outputEdit->append("Checking identity matrix...");
}

void MainMenuWidget::onRelaxationClick()
{
    int v = ui->verticesInput->text().toInt();
    QJsonArray edges = QJsonDocument::fromJson(ui->edgesInput->toPlainText().toUtf8()).array();

    QJsonObject req;
    req["action"] = "relaxation_graph";
    req["vertices"] = v;
    req["edges"] = edges;
    ClientAPI::instance().sendRequest(req);
    ui->outputEdit->append("Checking relaxation graph...");
}

void MainMenuWidget::onBipartiteClick()
{
    int v = ui->bVerticesInput->text().toInt();
    QJsonArray edges = QJsonDocument::fromJson(ui->bEdgesInput->toPlainText().toUtf8()).array();

    QJsonObject req;
    req["action"] = "bipartite_graph";
    req["vertices"] = v;
    req["edges"] = edges;
    ClientAPI::instance().sendRequest(req);
    ui->outputEdit->append("Checking bipartite graph...");
}

void MainMenuWidget::onResponseReceived(const QJsonObject &response)
{
    QString action = response["action"].toString();
    if (action == "matrix_identity_result") {
        ui->outputEdit->append("Result: " + response["message"].toString());
    } else if (action == "relaxation_graph_result") {
        ui->outputEdit->append("Result: " + response["message"].toString());
    } else if (action == "bipartite_graph_result") {
        ui->outputEdit->append("Result: " + response["message"].toString());
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
