#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtNetwork>
#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QString>

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

public slots:
    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();

private:
    QTcpServer * mTcpServer;
    QTcpSocket * mTcpSocket;

    QString calculateDegrees(const QString &incidenceMatrix);
    QList<QList<int>> parseMatrix(const QString &matrixStr);
};

#endif // MYTCPSERVER_H
