#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class MainMenuWidget;
}

class MainMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);
    ~MainMenuWidget();
    void setUserInfo(const QString &username, const QString &role);

signals:
    void logoutRequested();
    void loginSuccess(const QString &username, const QString &role);

private slots:
    void onTask1Clicked();
    void onTask2Clicked();
    void onTask3Clicked();
    void onTask4Clicked();
    void onResponseReceived(const QJsonObject &response);
    void onLogoutClick();

private:
    Ui::MainMenuWidget *ui;
};

#endif
