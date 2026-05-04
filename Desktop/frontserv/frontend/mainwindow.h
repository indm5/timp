#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

class AuthWidget;
class RegisterWidget;
class MainMenuWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void goToAuth();
    void goToRegister();
    void goToMainMenu(const QString &username, const QString &role);

private:
    QStackedWidget* m_stackedWidget;
    AuthWidget* m_authWidget;
    RegisterWidget* m_registerWidget;
    MainMenuWidget* m_mainMenuWidget;
};

#endif
