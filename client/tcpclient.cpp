#include <QCoreApplication>
#include <QTcpSocket>
#include <QDebug>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTcpSocket socket;
    QTextStream in(stdin);
    QTextStream out(stdout);

    out << "Подключение к серверу...\n";
    socket.connectToHost("127.0.0.1", 33333);

    if(!socket.waitForConnected(3000)) {
        out << "Ошибка подключения! Убедитесь, что сервер запущен.\n";
        return 1;
    }

    out << "Подключено к серверу!\n";
    out << "Введите матрицу инцидентности:\n";
    out << "Формат: числа через пробел, строки через точку с запятой\n";
    out << "Пример: 1 0 1; 1 1 0; 0 1 1\n";
    out << "Для выхода введите 'exit'\n";



    if(socket.waitForReadyRead(3000)) {
        QByteArray welcome = socket.readAll();
        out << QString::fromUtf8(welcome);
        out.flush();
    }

    while(socket.state() == QAbstractSocket::ConnectedState) {
        out << "> ";
        out.flush();

        QString input = in.readLine().trimmed();
        if(input.isEmpty()) {
            continue;
        }

        socket.write(input.toUtf8());
        socket.write("\n");
        socket.waitForBytesWritten(1000);

        if(input.toLower() == "exit") {
            break;
        }


        if(socket.waitForReadyRead(5000)) {
            while(socket.bytesAvailable()) {
                QByteArray response = socket.readAll();
                out << QString::fromUtf8(response);
                out.flush();
            }
        } else {
            out << "Время ожидания ответа от сервера истекло\n";
        }
    }

    socket.disconnectFromHost();
    out << "\nСоединение закрыто\n";

    return 0;
}
