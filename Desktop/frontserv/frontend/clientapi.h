#ifndef CLIENTAPI_H
#define CLIENTAPI_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class ClientAPI : public QObject
{
    Q_OBJECT

public:
    static ClientAPI& instance();
    void connectToServer(const QString &address, quint16 port);
    void disconnectFromServer();
    void sendRequest(const QJsonObject &request);
    bool getIsConnected() const { return m_isConnected; }

signals:
    void responseReceived(const QJsonObject &response);
    void errorOccurred(const QString &error);
    void serverConnected();
    void serverDisconnected();

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketErrorOccurred();

private:
    explicit ClientAPI(QObject *parent = nullptr);
    static ClientAPI* m_instance;
    QTcpSocket* m_socket;
    bool m_isConnected;
};

#endif
