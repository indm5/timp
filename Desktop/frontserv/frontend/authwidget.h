#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include "clientapi.h"

namespace Ui {
class AuthWidget;
}

class AuthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWidget(QWidget *parent = nullptr);
    ~AuthWidget();

signals:
    void loginSuccess(const QString &username, const QString &role);
    void switchToRegister();

private slots:
    void onLoginClicked();
    void onResponseReceived(const QJsonObject &response);

private:
    Ui::AuthWidget *ui;
};

#endif
