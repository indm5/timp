#include "functionsserver.h"
#include <QVector>
#include <QQueue>
#include <functional>

bool GraphFunctions::isIdentityMatrix(const QJsonArray &matrix, int size)
{
    for (int i = 0; i < size; i++) {
        QJsonArray row = matrix[i].toArray();
        for (int j = 0; j < size; j++) {
            int val = row[j].toInt();
            if (i == j && val != 1) return false;
            if (i != j && val != 0) return false;
        }
    }
    return true;
}

bool GraphFunctions::isRelaxationGraph(const QJsonArray &edges, int vertices)
{
    if (vertices <= 1) return true;
    if (edges.size() != vertices - 1) return false;

    QVector<QVector<int>> adj(vertices);
    for (const auto &e : edges) {
        QJsonArray a = e.toArray();
        int u = a[0].toInt(), v = a[1].toInt();
        adj[u].append(v);
        adj[v].append(u);
    }

    QVector<bool> visited(vertices, false);
    std::function<void(int)> dfs = [&](int node) {
        visited[node] = true;
        for (int nb : adj[node]) if (!visited[nb]) dfs(nb);
    };
    dfs(0);

    for (bool v : visited) if (!v) return false;
    return true;
}

bool GraphFunctions::isBipartiteGraph(const QJsonArray &edges, int vertices)
{
    if (vertices == 0) return true;

    QVector<QVector<int>> adj(vertices);
    for (const auto &e : edges) {
        QJsonArray a = e.toArray();
        int u = a[0].toInt(), v = a[1].toInt();
        adj[u].append(v);
        adj[v].append(u);
    }

    QVector<int> color(vertices, -1);
    QQueue<int> q;

    for (int i = 0; i < vertices; i++) {
        if (color[i] == -1) {
            q.enqueue(i);
            color[i] = 0;
            while (!q.isEmpty()) {
                int u = q.dequeue();
                for (int v : adj[u]) {
                    if (color[v] == -1) {
                        color[v] = color[u] ^ 1;
                        q.enqueue(v);
                    } else if (color[v] == color[u]) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
