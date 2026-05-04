#include "mainwindow.h"
#include "authwidget.h"
#include "registerwidget.h"
#include "mainmenuwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Graph Analysis System");
    setFixedSize(900, 700);

    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    m_authWidget = new AuthWidget(this);
    m_registerWidget = new RegisterWidget(this);
    m_mainMenuWidget = new MainMenuWidget(this);

    m_stackedWidget->addWidget(m_authWidget);
    m_stackedWidget->addWidget(m_registerWidget);
    m_stackedWidget->addWidget(m_mainMenuWidget);

    connect(m_authWidget, &AuthWidget::loginSuccess, this, &MainWindow::goToMainMenu);
    connect(m_authWidget, &AuthWidget::switchToRegister, this, &MainWindow::goToRegister);
    connect(m_registerWidget, &RegisterWidget::switchToLogin, this, &MainWindow::goToAuth);
    connect(m_registerWidget, &RegisterWidget::registrationSuccess, this, &MainWindow::goToAuth);
    connect(m_mainMenuWidget, &MainMenuWidget::logoutRequested, this, &MainWindow::goToAuth);

    goToAuth();
}

void MainWindow::goToAuth()
{
    m_stackedWidget->setCurrentWidget(m_authWidget);
}

void MainWindow::goToRegister()
{
    m_stackedWidget->setCurrentWidget(m_registerWidget);
}

void MainWindow::goToMainMenu(const QString &username, const QString &role)
{
    m_mainMenuWidget->setUserInfo(username, role);
    m_stackedWidget->setCurrentWidget(m_mainMenuWidget);
}

MainWindow::~MainWindow() {}
