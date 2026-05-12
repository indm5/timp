#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QJsonObject>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    bool start(quint16 port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QMap<QTcpSocket*, QByteArray> m_clients;

    void processRequest(QTcpSocket* client, const QByteArray &data);
    void sendResponse(QTcpSocket* client, const QJsonObject &response);

    // Обработчики задач
    void handleLogin(QTcpSocket* client, const QJsonObject &request);
    void handleRegister(QTcpSocket* client, const QJsonObject &request);
    void handleTask1(QTcpSocket* client, const QJsonObject &request);
    void handleTask2(QTcpSocket* client, const QJsonObject &request);
    void handleTask3(QTcpSocket* client, const QJsonObject &request);
    void handleTask4(QTcpSocket* client, const QJsonObject &request);
};

#endif
