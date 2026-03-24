#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <algorithm>

MyTcpServer::~MyTcpServer()
{
    if(mTcpServer)
        mTcpServer->close();
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "Сервер не запущен!";
    } else {
        qDebug() << "Сервер запущен на порту 33333";
        qDebug() << "Ожидание подключения клиентов...";
    }
}

void MyTcpServer::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();
    mTcpSocket->write("Добро пожаловать! Отправьте матрицу инцидентности.\n");
    mTcpSocket->write("Формат: числа через пробел, строки через точку с запятой\n");
    mTcpSocket->write("Пример: 1 0 1; 1 1 0; 0 1 1\n");

    connect(mTcpSocket, &QTcpSocket::readyRead, this, &MyTcpServer::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &MyTcpServer::slotClientDisconnected);

    qDebug() << "Новый клиент подключился!";
}

QList<QList<int>> MyTcpServer::parseMatrix(const QString &matrixStr)
{
    QList<QList<int>> matrix;
    QString trimmed = matrixStr.trimmed();

    QStringList rows = trimmed.split(';');

    for(int i = 0; i < rows.size(); ++i) {
        QStringList numbers = rows[i].trimmed().split(' ', Qt::SkipEmptyParts);
        QList<int> rowList;

        for(int j = 0; j < numbers.size(); ++j) {
            bool ok;
            int value = numbers[j].toInt(&ok);
            if(ok) {
                rowList.append(value);
            }
        }

        if(!rowList.isEmpty()) {
            matrix.append(rowList);
        }
    }

    return matrix;
}

QString MyTcpServer::calculateDegrees(const QString &incidenceMatrix)
{
    QString result;
    QList<QList<int>> matrix = parseMatrix(incidenceMatrix);

    if(matrix.isEmpty()) {
        return "Ошибка: пустая матрица!\n";
    }

    int numVertices = matrix.size();
    int numEdges = matrix[0].size();

    result += "=====================================\n";
    result += "Анализ матрицы инцидентности:\n";
    result += QString("Количество вершин: %1\n").arg(numVertices);
    result += QString("Количество ребер: %1\n").arg(numEdges);
    result += "=====================================\n\n";

    // Проверка корректности матрицы
    for(int i = 0; i < numVertices; i++) {
        if(matrix[i].size() != numEdges) {
            return QString("Ошибка: строка %1 имеет %2 элементов, ожидалось %3\n")
                .arg(i+1).arg(matrix[i].size()).arg(numEdges);
        }
    }

    result += "Степени вершин:\n";
    result += "-------------------------------------\n";

    QList<int> degrees;
    for(int i = 0; i < numVertices; i++) {
        int degree = 0;
        for(int j = 0; j < numEdges; j++) {
            if(matrix[i][j] != 0) {
                degree++;
            }
        }
        degrees.append(degree);
        result += QString("Вершина %1: степень = %2\n").arg(i+1).arg(degree);
    }

    result += "-------------------------------------\n";

    // Статистика
    result += "\nСтатистика:\n";
    if(!degrees.isEmpty()) {
        // Находим максимум и минимум
        int maxDeg = degrees[0];
        int minDeg = degrees[0];
        int sumDeg = 0;

        for(int i = 0; i < degrees.size(); ++i) {
            int currentDeg = degrees[i];
            if(currentDeg > maxDeg) maxDeg = currentDeg;
            if(currentDeg < minDeg) minDeg = currentDeg;
            sumDeg += currentDeg;
        }

        result += QString("Максимальная степень: %1\n").arg(maxDeg);
        result += QString("Минимальная степень: %1\n").arg(minDeg);
        result += QString("Сумма степеней: %1\n").arg(sumDeg);
        result += QString("Проверка: 2 * |E| = %1\n").arg(2 * numEdges);
        if(sumDeg == 2 * numEdges) {
            result += "✓ Сумма степеней соответствует 2*|E| (верно)\n";
        } else {
            result += "✗ Сумма степеней не соответствует 2*|E| (проверьте матрицу)\n";
        }
    }

    result += "=====================================\n";

    return result;
}

void MyTcpServer::slotServerRead()
{
    while(mTcpSocket->bytesAvailable() > 0)
    {
        QByteArray array = mTcpSocket->readAll();
        QString receivedData = QString::fromUtf8(array);

        qDebug() << "Получены данные от клиента:";
        qDebug() << receivedData;

        // Проверка на команду выхода
        if(receivedData.trimmed().toLower() == "exit" ||
            receivedData.trimmed().toLower() == "quit") {
            mTcpSocket->write("До свидания!\n");
            mTcpSocket->disconnectFromHost();
            break;
        }

        // Вычисляем степени вершин
        QString result = calculateDegrees(receivedData);

        // Отправляем результат клиенту
        mTcpSocket->write(result.toUtf8());
        mTcpSocket->write("\nДля завершения работы введите 'exit'\n");
        mTcpSocket->write("Для нового расчета отправьте новую матрицу:\n");
    }
}

void MyTcpServer::slotClientDisconnected()
{
    qDebug() << "Клиент отключился";
    if(mTcpSocket) {
        mTcpSocket->close();
    }
}
