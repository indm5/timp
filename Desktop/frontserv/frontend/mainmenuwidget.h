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

private slots:
    void onMatrixClick();
    void onRelaxationClick();
    void onBipartiteClick();
    void onResponseReceived(const QJsonObject &response);
    void onLogoutClick();

private:
    Ui::MainMenuWidget *ui;
};

#endif
