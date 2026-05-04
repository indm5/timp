#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

signals:
    void switchToLogin();
    void registrationSuccess();

private slots:
    void onRegisterClicked();
    void onResponseReceived(const QJsonObject &response);

private:
    Ui::RegisterWidget *ui;
};

#endif
