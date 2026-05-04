#include <QCoreApplication>
#include <QDebug>

// Включаем наш клиент‑синглтон
#include "singleton_client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SingletonClient* client = SingletonClient::getInstance();

    client->connectToServer("127.0.0.1", 33333);

    QObject::connect(client, &SingletonClient::message_from_server,
                     [](QString msg) {
                         qDebug() << "Сообщение от сервера:" << msg;
                     });

    return a.exec();
}
