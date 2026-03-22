#include "mytcpserver.h"

MyTcpServer::~MyTcpServer()
{
    mTcpServer->close();
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    initExampleGraph();

    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "Сервер не запущен! Ошибка!";
    } else {
        qDebug() << "Сервер запущен! Порт: 33333";
        qDebug() << "Матрица инцидентности загружена. Жду клиентов...";
    }
}

void MyTcpServer::initExampleGraph()
{
    incidenceMatrix.clear();

    QVector<int> vertex0 = {1, 1, 0};
    QVector<int> vertex1 = {1, 0, 1};
    QVector<int> vertex2 = {0, 1, 1};

    incidenceMatrix.append(vertex0);
    incidenceMatrix.append(vertex1);
    incidenceMatrix.append(vertex2);

    qDebug() << "Матрица инцидентности:";
    for(int i = 0; i < incidenceMatrix.size(); i++) {
        QString row = "Вершина " + QString::number(i) + ": ";
        for(int j = 0; j < incidenceMatrix[i].size(); j++) {
            row += QString::number(incidenceMatrix[i][j]) + " ";
        }
        qDebug() << row;
    }
}

int MyTcpServer::calculateVertexDegree(int vertexIndex)
{
    if(vertexIndex < 0 || vertexIndex >= incidenceMatrix.size()) {
        return -1;
    }

    int degree = 0;
    for(int i = 0; i < incidenceMatrix[vertexIndex].size(); i++) {
        degree += incidenceMatrix[vertexIndex][i];
    }
    return degree;
}

void MyTcpServer::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();

    qDebug() << "Клиент подключился!";

    mTcpSocket->write("Добро пожаловать! Введите номер вершины (0, 1, 2...):\r\n");

    connect(mTcpSocket, &QTcpSocket::readyRead,
            this, &MyTcpServer::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected,
            this, &MyTcpServer::slotClientDisconnected);
}

void MyTcpServer::slotServerRead()
{
    while(mTcpSocket->bytesAvailable() > 0)
    {
        QByteArray data = mTcpSocket->readAll();
        QString message = QString(data).trimmed();

        qDebug() << "Получено от клиента:" << message;

        bool ok;
        int vertexNumber = message.toInt(&ok);

        if(ok) {
            int degree = calculateVertexDegree(vertexNumber);

            if(degree >= 0) {
                QString response = "Степень вершины " + QString::number(vertexNumber) +
                                   " равна " + QString::number(degree) + "\r\n";
                mTcpSocket->write(response.toUtf8());
            } else {
                mTcpSocket->write("Ошибка: вершины с таким номером не существует!\r\n");
            }
        } else {
            mTcpSocket->write("Ошибка: введите число - номер вершины!\r\n");
        }

        mTcpSocket->write("Введите номер вершины (0, 1, 2...):\r\n");
    }
}

void MyTcpServer::slotClientDisconnected()
{
    qDebug() << "Клиент отключился";
    mTcpSocket->close();
}
