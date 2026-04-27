#include "mytcpserver.h"
#include "server_functions.h"

MyTcpServer::~MyTcpServer()
{
    for (auto socket : mSockets) {
        socket->close();
        socket->deleteLater();
    }
    mSockets.clear();
    mTcpServer->close();
}

MyTcpServer::MyTcpServer(QObject *parent): QObject(parent),
    mTcpServer(new QTcpServer(this))
{
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if (!mTcpServer->listen(QHostAddress::Any, 33333)) {
        qDebug() << "Server is not started";
    } else {
        qDebug() << "Server is started on port 33333";
    }
}

void MyTcpServer::slotNewConnection(){
    while (mTcpServer->hasPendingConnections()) {
        QTcpSocket *mTcpSocket;
        mTcpSocket = mTcpServer->nextPendingConnection();
        mTcpSocket->write("Hello, World!!! I am echo server!\r\n");
        connect(mTcpSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
        connect(mTcpSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);
        mSockets[mTcpSocket->socketDescriptor()] = mTcpSocket;
    }
}

void MyTcpServer::slotServerRead(){
    QTcpSocket *mTcpSocket = qobject_cast<QTcpSocket*>(sender());
    if (!mTcpSocket)
        return;

    QString res = "";
    while(mTcpSocket->bytesAvailable() > 0)
    {
        QByteArray array = mTcpSocket->readAll();
        qDebug() << array << "\n";
        if(array == "\x01")
        {
            mTcpSocket->write(parsing(res).toUtf8());
            res = "";
        }
        else
            res.append(array);
    }
    mTcpSocket->write(parsing(res).toUtf8());
}

void MyTcpServer::slotClientDisconnected(){
    QTcpSocket *mTcpSocket = qobject_cast<QTcpSocket*>(sender());
    if (!mTcpSocket)
        return;

    mSockets.remove(mTcpSocket->socketDescriptor());
    mTcpSocket->close();
    mTcpSocket->deleteLater();
}
