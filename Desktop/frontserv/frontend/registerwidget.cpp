#include "registerwidget.h"
#include "ui_registerwidget.h"
#include "clientapi.h"

RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);

    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegisterClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &RegisterWidget::switchToLogin);
    connect(&ClientAPI::instance(), &ClientAPI::responseReceived, this, &RegisterWidget::onResponseReceived);
}

void RegisterWidget::onRegisterClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString confirmPassword = ui->confirmPasswordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("Please fill all fields");
        return;
    }

    if (password != confirmPassword) {
        ui->statusLabel->setText("Passwords do not match");
        return;
    }

    QJsonObject request;
    request["action"] = "register";
    request["username"] = username;
    request["password"] = password;

    ClientAPI::instance().sendRequest(request);
    ui->registerButton->setEnabled(false);
    ui->statusLabel->setText("Registering...");
}

void RegisterWidget::onResponseReceived(const QJsonObject &response)
{
    ui->registerButton->setEnabled(true);

    QString action = response["action"].toString();
    if (action == "register_response") {
        if (response["status"] == "success") {
            ui->statusLabel->setText("Registration successful! Please login.");
            emit registrationSuccess();
        } else {
            ui->statusLabel->setText(response["message"].toString());
        }
    }
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}
