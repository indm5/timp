#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QJsonArray>

namespace GraphFunctions {
bool isIdentityMatrix(const QJsonArray &matrix, int size);
bool isRelaxationGraph(const QJsonArray &edges, int vertices);
bool isBipartiteGraph(const QJsonArray &edges, int vertices);
}

#endif
