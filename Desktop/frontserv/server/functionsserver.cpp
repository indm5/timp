#include "functionsserver.h"
#include <QVector>
#include <QQueue>
#include <QDebug>

using namespace GraphFunctions;

// ЗАДАЧА 1: Степени всех вершин по матрице инцидентности
QJsonObject GraphFunctions::task1_VertexDegrees(const QJsonArray &incidenceMatrix, int vertices, int edges)
{
    QJsonObject result;
    QJsonArray degreesArray;

    // Подсчет степеней вершин
    for (int i = 0; i < vertices; i++) {
        int degree = 0;
        QJsonArray row = incidenceMatrix[i].toArray();
        for (int j = 0; j < edges && j < row.size(); j++) {
            int value = row[j].toInt();
            if (value != 0) {
                degree++;
            }
        }
        degreesArray.append(degree);
    }

    result["status"] = "success";
    result["action"] = "task1_result";
    result["degrees"] = degreesArray;

    QString msg = "Степени вершин: ";
    for (int i = 0; i < degreesArray.size(); i++) {
        msg += QString("v%1=%2 ").arg(i).arg(degreesArray[i].toInt());
    }
    result["message"] = msg;

    return result;
}

// ЗАДАЧА 2: Релаксация ребра
QJsonObject GraphFunctions::task2_RelaxEdge(int currentDistance, int edgeWeight)
{
    QJsonObject result;

    int newDistance = currentDistance + edgeWeight;
    bool wasRelaxed = (newDistance < currentDistance) || (currentDistance == 999999);

    result["status"] = "success";
    result["action"] = "task2_result";
    result["current_distance"] = currentDistance;
    result["edge_weight"] = edgeWeight;
    result["new_distance"] = newDistance;
    result["was_relaxed"] = wasRelaxed;
    result["message"] = wasRelaxed ?
                            QString("Релаксация выполнена! Расстояние изменилось с %1 на %2").arg(currentDistance).arg(newDistance) :
                            QString("Релаксация НЕ выполнена. Расстояние %1 не улучшается").arg(currentDistance);

    return result;
}

// ЗАДАЧА 3: Разбиение двудольного графа на доли
QJsonObject GraphFunctions::task3_BipartiteParts(const QJsonArray &edges, int vertices)
{
    QJsonObject result;
    QJsonArray partA, partB;

    if (vertices == 0) {
        result["status"] = "error";
        result["message"] = "Граф не содержит вершин";
        return result;
    }

    // Построение списка смежности
    QVector<QVector<int>> adj(vertices);
    for (const auto &edge : edges) {
        QJsonArray edgeArray = edge.toArray();
        if (edgeArray.size() < 2) continue;
        int u = edgeArray[0].toInt();
        int v = edgeArray[1].toInt();
        if (u >= 0 && u < vertices && v >= 0 && v < vertices) {
            adj[u].append(v);
            adj[v].append(u);
        }
    }

    // BFS раскраска в 2 цвета
    QVector<int> color(vertices, -1);
    QQueue<int> q;
    bool isBipartite = true;

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
                        isBipartite = false;
                        break;
                    }
                }
                if (!isBipartite) break;
            }
        }
        if (!isBipartite) break;
    }

    if (!isBipartite) {
        result["status"] = "error";
        result["action"] = "task3_result";
        result["is_bipartite"] = false;
        result["message"] = "Граф НЕ является двудольным! Разбиение на доли невозможно.";
        return result;
    }

    // Формирование долей
    for (int i = 0; i < vertices; i++) {
        if (color[i] == 0) {
            partA.append(i);
        } else {
            partB.append(i);
        }
    }

    result["status"] = "success";
    result["action"] = "task3_result";
    result["is_bipartite"] = true;
    result["partA"] = partA;
    result["partB"] = partB;

    QString aStr, bStr;
    for (int i = 0; i < partA.size(); i++) aStr += QString::number(partA[i].toInt()) + " ";
    for (int i = 0; i < partB.size(); i++) bStr += QString::number(partB[i].toInt()) + " ";
    result["message"] = QString("Граф двудольный! Доля A: [%1] Доля B: [%2]").arg(aStr).arg(bStr);

    return result;
}

// ЗАДАЧА 4: Существует ли путь между двумя вершинами
QJsonObject GraphFunctions::task4_HasPath(const QJsonArray &edges, int vertices, int start, int end)
{
    QJsonObject result;

    if (start < 0 || start >= vertices || end < 0 || end >= vertices) {
        result["status"] = "error";
        result["message"] = "Некорректные номера вершин";
        return result;
    }

    if (start == end) {
        result["status"] = "success";
        result["action"] = "task4_result";
        result["has_path"] = true;
        result["message"] = "Путь существует (начальная и конечная вершины совпадают)";
        return result;
    }

    // Построение списка смежности
    QVector<QVector<int>> adj(vertices);
    for (const auto &edge : edges) {
        QJsonArray edgeArray = edge.toArray();
        if (edgeArray.size() < 2) continue;
        int u = edgeArray[0].toInt();
        int v = edgeArray[1].toInt();
        if (u >= 0 && u < vertices && v >= 0 && v < vertices) {
            adj[u].append(v);
            adj[v].append(u);
        }
    }

    // BFS поиск пути
    QVector<bool> visited(vertices, false);
    QQueue<int> q;
    q.enqueue(start);
    visited[start] = true;

    while (!q.isEmpty()) {
        int u = q.dequeue();
        for (int v : adj[u]) {
            if (!visited[v]) {
                if (v == end) {
                    result["status"] = "success";
                    result["action"] = "task4_result";
                    result["has_path"] = true;
                    result["message"] = QString("Путь между вершинами %1 и %2 существует").arg(start).arg(end);
                    return result;
                }
                visited[v] = true;
                q.enqueue(v);
            }
        }
    }

    result["status"] = "success";
    result["action"] = "task4_result";
    result["has_path"] = false;
    result["message"] = QString("Путь между вершинами %1 и %2 НЕ существует").arg(start).arg(end);

    return result;
}
