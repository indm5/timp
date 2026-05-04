#include <QCoreApplication>
#include <QDebug>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "========================================";
    qDebug() << "Graph Analysis Server Starting...";
    qDebug() << "========================================";

    Server server;
    if (server.start(12345)) {
        qDebug() << "[OK] Server started on port 12345";
        qDebug() << "[INFO] Waiting for connections...";
    } else {
        qDebug() << "[ERROR] Failed to start server";
        return 1;
    }

    return a.exec();
}
