#include "mytcpserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <queue>

MyTcpServer::MyTcpServer(QObject *parent)
    : QTcpServer(parent)
{
}

MyTcpServer::~MyTcpServer()
{
    for (QTcpSocket *socket : clients) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
}

bool MyTcpServer::startServer(quint16 port)
{
    if (!listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start on port" << port;
        return false;
    }
    qDebug() << "Server started on port" << port;
    qDebug() << "Waiting for connections...";
    return true;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    clients.append(socket);

    connect(socket, &QTcpSocket::readyRead, this, &MyTcpServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &MyTcpServer::onDisconnected);

    qDebug() << "New client connected from" << socket->peerAddress().toString();
}

void MyTcpServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isObject()) {
        QJsonObject request = doc.object();
        QString type = request["type"].toString();
        QString dataStr = request["data"].toString();

        qDebug() << "Received request:" << type;

        QString result;
        bool success = true;

        if (type == "bipartite") {
            result = checkBipartite(dataStr);
        } else if (type == "degrees") {
            result = calculateDegrees(dataStr);
        } else if (type == "relaxation") {
            QStringList parts = dataStr.split("|");
            if (parts.size() == 2) {
                result = relaxEdge(parts[0], parts[1]);
            } else {
                result = "Ошибка: неверный формат данных";
                success = false;
            }
        } else {
            result = "Неизвестный тип запроса";
            success = false;
        }

        // Отправляем ответ
        QJsonObject response;
        response["result"] = result;
        response["success"] = success;

        QJsonDocument responseDoc(response);
        socket->write(responseDoc.toJson());
        socket->flush();

        qDebug() << "Response sent, success:" << success;
    }
}

void MyTcpServer::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        clients.removeAll(socket);
        socket->deleteLater();
        qDebug() << "Client disconnected";
    }
}

// Вычисления

QString MyTcpServer::checkBipartite(const QString &adjMatrix)
{
    QVector<QVector<int>> graph = parseAdjacencyMatrix(adjMatrix);

    if (graph.isEmpty()) {
        return "Ошибка: неверный формат матрицы смежности\nПример:\n0 1 0;1 0 1;0 1 0";
    }

    int n = graph.size();
    QVector<int> color(n, -1);
    QVector<int> part1, part2;

    for (int i = 0; i < n; i++) {
        if (color[i] == -1) {
            std::queue<int> q;
            q.push(i);
            color[i] = 0;
            part1.append(i);

            while (!q.empty()) {
                int u = q.front();
                q.pop();

                for (int v = 0; v < n; v++) {
                    if (graph[u][v] == 1) {
                        if (color[v] == -1) {
                            color[v] = color[u] ^ 1;
                            q.push(v);
                            if (color[v] == 0) part1.append(v);
                            else part2.append(v);
                        } else if (color[v] == color[u]) {
                            return "Граф НЕ является двудольным (найдено ребро внутри доли)";
                        }
                    }
                }
            }
        }
    }

    QString result = "Граф является двудольным!\n\n";
    result += "Доля 1 (вершины): ";
    for (int i = 0; i < part1.size(); i++) {
        result += QString::number(part1[i] + 1);
        if (i < part1.size() - 1) result += ", ";
    }

    result += "\nДоля 2 (вершины): ";
    for (int i = 0; i < part2.size(); i++) {
        result += QString::number(part2[i] + 1);
        if (i < part2.size() - 1) result += ", ";
    }

    return result;
}

QString MyTcpServer::calculateDegrees(const QString &incMatrix)
{
    QVector<QVector<int>> matrix = parseIncidenceMatrix(incMatrix);

    if (matrix.isEmpty()) {
        return "Ошибка: неверный формат матрицы инцидентности\nПример:\n1 1 0;1 0 1;0 1 1";
    }

    int vertices = matrix.size();
    if (vertices == 0) {
        return "Ошибка: нет вершин";
    }

    int edges = matrix[0].size();
    QVector<int> degrees(vertices, 0);

    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < edges; j++) {
            if (matrix[i][j] == 1) {
                degrees[i]++;
            }
        }
    }

    QString result = "Степени вершин:\n";
    result += "━━━━━━━━━━━━━━━━━━━━\n";
    for (int i = 0; i < vertices; i++) {
        result += QString("Вершина %1: степень %2\n").arg(i + 1).arg(degrees[i]);
    }
    result += "━━━━━━━━━━━━━━━━━━━━\n";
    result += QString("Всего вершин: %1\n").arg(vertices);
    result += QString("Всего рёбер: %1").arg(edges);

    return result;
}

QString MyTcpServer::relaxEdge(const QString &distances, const QString &edge)
{
    // Парсим расстояния
    QStringList distList = distances.split(" ", Qt::SkipEmptyParts);
    QVector<int> dist;
    for (const QString &d : distList) {
        bool ok;
        int val = d.toInt(&ok);
        if (!ok) {
            return "Ошибка: неверный формат расстояний\nПример: 0 5 100 100";
        }
        dist.append(val);
    }

    // Парсим ребро
    QStringList edgeList = edge.split(" ", Qt::SkipEmptyParts);
    if (edgeList.size() != 3) {
        return "Ошибка: ребро должно содержать u, v и w (через пробел)\nПример: 1 3 10";
    }

    bool ok1, ok2, ok3;
    int u = edgeList[0].toInt(&ok1) - 1;
    int v = edgeList[1].toInt(&ok2) - 1;
    int w = edgeList[2].toInt(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        return "Ошибка: неверный формат ребра";
    }

    if (u < 0 || u >= dist.size() || v < 0 || v >= dist.size()) {
        return QString("Ошибка: вершины должны быть от 1 до %1").arg(dist.size());
    }

    // Выполняем релаксацию
    QString result;
    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    result += "РЕЛАКСАЦИЯ РЕБРА\n";
    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    result += "Исходные расстояния: [";
    for (int i = 0; i < dist.size(); i++) {
        if (dist[i] == 1000000) result += "∞";
        else result += QString::number(dist[i]);
        if (i < dist.size() - 1) result += ", ";
    }
    result += "]\n\n";

    result += QString("Ребро (%1 → %2) с весом %3\n\n").arg(u + 1).arg(v + 1).arg(w);

    if (dist[u] + w < dist[v]) {
        int oldDist = dist[v];
        dist[v] = dist[u] + w;
        result += "РЕЛАКСАЦИЯ ВЫПОЛНЕНА!\n";
        result += QString("Расстояние до вершины %1 улучшено: ").arg(v + 1);
        if (oldDist == 1000000) result += "∞";
        else result += QString::number(oldDist);
        result += QString(" → %1\n").arg(dist[v]);
    } else {
        result += "Релаксация не выполнена:\n";
        result += QString("   %1 + %2 >= %3\n").arg(dist[u]).arg(w).arg(dist[v]);
    }

    result += "\nНовые расстояния: [";
    for (int i = 0; i < dist.size(); i++) {
        if (dist[i] == 1000000) result += "∞";
        else result += QString::number(dist[i]);
        if (i < dist.size() - 1) result += ", ";
    }
    result += "]\n";
    result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━";

    return result;
}

QVector<QVector<int>> MyTcpServer::parseAdjacencyMatrix(const QString &input)
{
    QVector<QVector<int>> matrix;
    QStringList rows = input.split(";", Qt::SkipEmptyParts);

    for (const QString &row : rows) {
        QStringList values = row.split(" ", Qt::SkipEmptyParts);
        QVector<int> rowVec;
        for (const QString &val : values) {
            bool ok;
            int num = val.toInt(&ok);
            if (!ok || (num != 0 && num != 1)) {
                return QVector<QVector<int>>();
            }
            rowVec.append(num);
        }
        matrix.append(rowVec);
    }

    if (matrix.isEmpty()) return matrix;
    int size = matrix.size();
    for (const auto &row : matrix) {
        if (row.size() != size) {
            return QVector<QVector<int>>();
        }
    }

    return matrix;
}

QVector<QVector<int>> MyTcpServer::parseIncidenceMatrix(const QString &input)
{
    QVector<QVector<int>> matrix;
    QStringList rows = input.split(";", Qt::SkipEmptyParts);

    if (rows.isEmpty()) return matrix;

    int numEdges = -1;
    for (const QString &row : rows) {
        QStringList values = row.split(" ", Qt::SkipEmptyParts);
        QVector<int> rowVec;
        for (const QString &val : values) {
            bool ok;
            int num = val.toInt(&ok);
            if (!ok || (num != 0 && num != 1)) {
                return QVector<QVector<int>>();
            }
            rowVec.append(num);
        }

        if (numEdges == -1) {
            numEdges = rowVec.size();
        } else if (rowVec.size() != numEdges) {
            return QVector<QVector<int>>();
        }

        matrix.append(rowVec);
    }

    return matrix;
}
