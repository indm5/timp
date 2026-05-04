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
    } else if (action == "matrix_identity") {
        handleMatrixIdentity(client, request);
    } else if (action == "relaxation_graph") {
        handleRelaxationGraph(client, request);
    } else if (action == "bipartite_graph") {
        handleBipartiteGraph(client, request);
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

void Server::handleMatrixIdentity(QTcpSocket* client, const QJsonObject &request)
{
    QJsonArray matrix = request["matrix"].toArray();
    int size = request["size"].toInt();

    bool result = GraphFunctions::isIdentityMatrix(matrix, size);

    QJsonObject response;
    response["action"] = "matrix_identity_result";
    response["is_identity"] = result;
    response["message"] = result ? "Matrix is identity matrix" : "Matrix is NOT identity matrix";

    sendResponse(client, response);
}

void Server::handleRelaxationGraph(QTcpSocket* client, const QJsonObject &request)
{
    QJsonArray edges = request["edges"].toArray();
    int vertices = request["vertices"].toInt();

    bool result = GraphFunctions::isRelaxationGraph(edges, vertices);

    QJsonObject response;
    response["action"] = "relaxation_graph_result";
    response["is_relaxed"] = result;
    response["message"] = result ? "Graph is relaxed (is a tree)" : "Graph is NOT relaxed";

    sendResponse(client, response);
}

void Server::handleBipartiteGraph(QTcpSocket* client, const QJsonObject &request)
{
    QJsonArray edges = request["edges"].toArray();
    int vertices = request["vertices"].toInt();

    bool result = GraphFunctions::isBipartiteGraph(edges, vertices);

    QJsonObject response;
    response["action"] = "bipartite_graph_result";
    response["is_bipartite"] = result;
    response["message"] = result ? "Graph is bipartite" : "Graph is NOT bipartite";

    sendResponse(client, response);
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
