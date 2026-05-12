#include "server.h"
#include "database.h"
#include "functionsserver.h"
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

Server::Server(QObject *parent) : QTcpServer(parent)
{
    qDebug() << "[INFO] Initializing server...";
    Database::instance().initialize();
}

bool Server::start(quint16 port)
{
    if (!listen(QHostAddress::Any, port)) {
        qDebug() << "[ERROR] Failed to bind to port" << port;
        return false;
    }
    return true;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *client = new QTcpSocket(this);

    if (!client->setSocketDescriptor(socketDescriptor)) {
        client->deleteLater();
        return;
    }

    qDebug() << "[INFO] Client connected:" << client->peerAddress().toString();
    m_clients[client] = QByteArray();

    connect(client, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &Server::onDisconnected);
}

void Server::onReadyRead()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QByteArray data = client->readAll();
    m_clients[client].append(data);

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(m_clients[client], &parseError);

    if (parseError.error == QJsonParseError::NoError) {
        processRequest(client, m_clients[client]);
        m_clients[client].clear();
    }
}

void Server::processRequest(QTcpSocket* client, const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject request = doc.object();
    QString action = request["action"].toString();

    qDebug() << "[REQUEST]" << action;

    if (action == "login") {
        handleLogin(client, request);
    } else if (action == "register") {
        handleRegister(client, request);
    } else if (action == "task1") {
        handleTask1(client, request);
    } else if (action == "task2") {
        handleTask2(client, request);
    } else if (action == "task3") {
        handleTask3(client, request);
    } else if (action == "task4") {
        handleTask4(client, request);
    }
}

void Server::sendResponse(QTcpSocket* client, const QJsonObject &response)
{
    QJsonDocument doc(response);
    client->write(doc.toJson());
    client->flush();
}

void Server::handleLogin(QTcpSocket* client, const QJsonObject &request)
{
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    QJsonObject response;
    response["action"] = "login_response";

    if (Database::instance().validateUser(username, password)) {
        response["status"] = "success";
        response["username"] = username;
        response["role"] = Database::instance().getUserRole(username);
        response["message"] = "Login successful";
    } else {
        response["status"] = "error";
        response["message"] = "Invalid username or password";
    }

    sendResponse(client, response);
}

void Server::handleRegister(QTcpSocket* client, const QJsonObject &request)
{
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    QJsonObject response;
    response["action"] = "register_response";

    if (Database::instance().registerUser(username, password)) {
        response["status"] = "success";
        response["message"] = "Registration successful";
    } else {
        response["status"] = "error";
        response["message"] = "Username already exists";
    }

    sendResponse(client, response);
}

// ЗАДАЧА 1: Степени вершин по матрице инцидентности
void Server::handleTask1(QTcpSocket* client, const QJsonObject &request)
{
    QJsonArray incidenceMatrix = request["incidenceMatrix"].toArray();
    int vertices = request["vertices"].toInt();
    int edges = request["edges"].toInt();

    QJsonObject result = GraphFunctions::task1_VertexDegrees(incidenceMatrix, vertices, edges);
    sendResponse(client, result);
}

// ЗАДАЧА 2: Релаксация ребра
void Server::handleTask2(QTcpSocket* client, const QJsonObject &request)
{
    int currentDistance = request["currentDistance"].toInt();
    int edgeWeight = request["edgeWeight"].toInt();

    QJsonObject result = GraphFunctions::task2_RelaxEdge(currentDistance, edgeWeight);
    sendResponse(client, result);
}

// ЗАДАЧА 3: Разбиение двудольного графа на доли
void Server::handleTask3(QTcpSocket* client, const QJsonObject &request)
{
    QJsonArray edges = request["edges"].toArray();
    int vertices = request["vertices"].toInt();

    QJsonObject result = GraphFunctions::task3_BipartiteParts(edges, vertices);
    sendResponse(client, result);
}

// ЗАДАЧА 4: Существует ли путь между вершинами
void Server::handleTask4(QTcpSocket* client, const QJsonObject &request)
{
    QJsonArray edges = request["edges"].toArray();
    int vertices = request["vertices"].toInt();
    int start = request["start"].toInt();
    int end = request["end"].toInt();

    QJsonObject result = GraphFunctions::task4_HasPath(edges, vertices, start, end);
    sendResponse(client, result);
}

void Server::onDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        qDebug() << "[INFO] Client disconnected:" << client->peerAddress().toString();
        m_clients.remove(client);
        client->deleteLater();
    }
}
