#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBtnSolve1Clicked();
    void onBtnSolve2Clicked();
    void onBtnSolve3Clicked();
    void onServerResponse();
    void onConnected();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QString currentTask;
    void sendToServer(const QString &type, const QString &data);
};

#endif // MAINWINDOW_H
