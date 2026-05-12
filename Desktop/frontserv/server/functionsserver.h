#ifndef FUNCTIONSERVER_H
#define FUNCTIONSERVER_H

#include <QJsonArray>
#include <QJsonObject>

namespace GraphFunctions {
// Задача 1: степени всех вершин по матрице инцидентности
QJsonObject task1_VertexDegrees(const QJsonArray &incidenceMatrix, int vertices, int edges);

// Задача 2: релаксация ребра
QJsonObject task2_RelaxEdge(int currentDistance, int edgeWeight);

// Задача 3: разбиение двудольного графа на доли
QJsonObject task3_BipartiteParts(const QJsonArray &edges, int vertices);

// Задача 4: существует ли путь между двумя вершинами
QJsonObject task4_HasPath(const QJsonArray &edges, int vertices, int start, int end);
}

#endif
