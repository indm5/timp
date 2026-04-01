#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include<QString>
#include <QStringList>

MyTcpServer::~MyTcpServer()
{

    mTcpServer->close();
    //server_status=0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
 //   if(server_status==1){
        mTcpSocket = mTcpServer->nextPendingConnection();
        mTcpSocket->write("Hello, World!!! I am echo server!\r\n");
        connect(mTcpSocket, &QTcpSocket::readyRead,this,&MyTcpServer::slotServerRead);
        connect(mTcpSocket,&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);
   // }
}

void MyTcpServer::slotServerRead(){
    // QString res = "";  раньше тут копили текст пока не получим \x01
    while(mTcpSocket->bytesAvailable()>0)
    {
        QByteArray array =mTcpSocket->readAll();

        QString query = QString::fromUtf8(array).trimmed(); //превращаем байты в строку и убираем лишние пробелы и переносы строк
        qDebug() << "Получен запрос от клиента:" << query;

        QByteArray response = parse(query); //отправляем строку на парсинг и получаем ответ заглушку

        mTcpSocket->write(response); //отправляем ответ клиенту

        /* qDebug()<<array<<"\n";
        if(array=="\x01")
        {
            mTcpSocket->write(res.toUtf8());
            res = "";
        }
        else
            res.append(array);
    }
    mTcpSocket->write(res.toUtf8()); */
    }

}

    QByteArray MyTcpServer::parse(QString query){

        QStringList params = query.split('&'); //делим строку символом &
        QString command = params[0]; //первое слово это всегда команда

            if (command == "auth") {
            return "AUTH_SUCCESS\r\n"; //заглушка (типо проверка логина и пароля)
        }

        else if (command == "reg") {
            return "REG_SUCCESS\r\n"; //заглушка регистрация нового пользователя
        }

        else if (command == "stats") {
            return "STATS_DATA: win=10; loss=2; rank=gold\r\n"; //заглушка функционал
        }
        else {
            return "UNKNOWN_COMMAND\r\n"; //в случае неизвестной команды
        }

}

void MyTcpServer::slotClientDisconnected(){
    mTcpSocket->close();
}
