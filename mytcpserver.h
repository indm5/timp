#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

    bool startServer(quint16 port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QVector<QTcpSocket*> clients;
    QString checkBipartite(const QString &adjMatrix);
    QString calculateDegrees(const QString &incMatrix);
    QString relaxEdge(const QString &distances, const QString &edge);

    QVector<QVector<int>> parseAdjacencyMatrix(const QString &input);
    QVector<QVector<int>> parseIncidenceMatrix(const QString &input);
};

#endif // MYTCPSERVER_H
