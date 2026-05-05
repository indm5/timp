#include "authwidget.h"
#include "ui_authwidget.h"


AuthWidget::AuthWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AuthWidget)
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked, this, &AuthWidget::onLoginClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &AuthWidget::switchToRegister);
    connect(&ClientAPI::instance(), &ClientAPI::responseReceived, this, &AuthWidget::onResponseReceived);

    ClientAPI::instance().connectToServer("127.0.0.1", 12345);
}

void AuthWidget::onLoginClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("Please enter username and password");
        return;
    }

    QJsonObject request;
    request["action"] = "login";
    request["username"] = username;
    request["password"] = password;

    ClientAPI::instance().sendRequest(request);
    ui->loginButton->setEnabled(false);
    ui->statusLabel->setText("Logging in...");
}

void AuthWidget::onResponseReceived(const QJsonObject &response)
{
    ui->loginButton->setEnabled(true);

    if (response["action"] == "login_response") {
        if (response["status"] == "success") {
            emit loginSuccess(response["username"].toString(), response["role"].toString());
        } else {
            ui->statusLabel->setText(response["message"].toString());
        }
    }
}

AuthWidget::~AuthWidget()
{
    delete ui;
}
