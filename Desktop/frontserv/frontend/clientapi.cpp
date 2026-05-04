#include "clientapi.h"
#include <QJsonDocument>
#include <QDebug>

ClientAPI* ClientAPI::m_instance = nullptr;

ClientAPI::ClientAPI(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_isConnected(false)
{
    connect(m_socket, &QTcpSocket::connected, this, &ClientAPI::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientAPI::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientAPI::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &ClientAPI::onSocketErrorOccurred);
}

ClientAPI& ClientAPI::instance()
{
    if (!m_instance) {
        m_instance = new ClientAPI();
    }
    return *m_instance;
}

void ClientAPI::connectToServer(const QString &address, quint16 port)
{
    if (!m_isConnected) {
        m_socket->connectToHost(address, port);
    }
}

void ClientAPI::disconnectFromServer()
{
    if (m_socket->isOpen()) {
        m_socket->disconnectFromHost();
    }
    m_isConnected = false;
}

void ClientAPI::sendRequest(const QJsonObject &request)
{
    if (!m_isConnected) {
        emit errorOccurred("Not connected to server");
        return;
    }
    QJsonDocument doc(request);
    m_socket->write(doc.toJson());
    m_socket->flush();
}

void ClientAPI::onSocketConnected()
{
    m_isConnected = true;
    emit serverConnected();
    qDebug() << "Connected to server";
}

void ClientAPI::onSocketDisconnected()
{
    m_isConnected = false;
    emit serverDisconnected();
    qDebug() << "Disconnected from server";
}

void ClientAPI::onSocketReadyRead()
{
    QByteArray data = m_socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject()) {
        emit responseReceived(doc.object());
    }
}

void ClientAPI::onSocketErrorOccurred()
{
    m_isConnected = false;
    emit errorOccurred(m_socket->errorString());
    qDebug() << "Socket error:" << m_socket->errorString();
}
