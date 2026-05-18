#include <QtTest>
#include <QJsonArray>
#include <QJsonObject>
#include "functionsserver.h"

using namespace GraphFunctions;

class TestGraphFunctions : public QObject
{
    Q_OBJECT

private slots:
    void testTask1_SimpleGraph();
    void testTask1_EmptyGraph();
    void testTask2_RelaxPossible();
    void testTask2_RelaxNotPossible();
    void testTask3_BipartiteGraph();
    void testTask3_NonBipartiteGraph();
    void testTask4_PathExists();
    void testTask4_NoPathExists();
};

// степени всех вершин по матрице инцидентности

void TestGraphFunctions::testTask1_SimpleGraph()
{
    QJsonArray incidenceMatrix;
    incidenceMatrix.append(QJsonArray{1, 0});
    incidenceMatrix.append(QJsonArray{1, 1});
    incidenceMatrix.append(QJsonArray{0, 1});

    QJsonObject result = task1_VertexDegrees(incidenceMatrix, 3, 2);

    QCOMPARE(result["status"].toString(), QString("success"));
    QJsonArray degrees = result["degrees"].toArray();
    QCOMPARE(degrees.size(), 3);
    QCOMPARE(degrees[0].toInt(), 1);
    QCOMPARE(degrees[1].toInt(), 2);
    QCOMPARE(degrees[2].toInt(), 1);
}

void TestGraphFunctions::testTask1_EmptyGraph()
{
    QJsonArray incidenceMatrix;
    QJsonObject result = task1_VertexDegrees(incidenceMatrix, 0, 0);
    QCOMPARE(result["status"].toString(), QString("success"));
    QCOMPARE(result["degrees"].toArray().size(), 0);
}

//Релаксация ребра
void TestGraphFunctions::testTask2_RelaxPossible()
{
    QJsonObject result = task2_RelaxEdge(999999, 30);
    QVERIFY(result["was_relaxed"].toBool());
    QCOMPARE(result["new_distance"].toInt(), 999999 + 30);
}

void TestGraphFunctions::testTask2_RelaxNotPossible()
{
    QJsonObject result = task2_RelaxEdge(50, 100);
    QVERIFY(!result["was_relaxed"].toBool());
    QCOMPARE(result["new_distance"].toInt(), 150);
}

//Разбиение двудольного графа на доли
//двудольный граф
void TestGraphFunctions::testTask3_BipartiteGraph()
{
    QJsonArray edges;
    edges.append(QJsonArray{0, 2});
    edges.append(QJsonArray{0, 3});
    edges.append(QJsonArray{1, 2});
    edges.append(QJsonArray{1, 3});

    QJsonObject result = task3_BipartiteParts(edges, 4);

    QCOMPARE(result["status"].toString(), QString("success"));
    QVERIFY(result["is_bipartite"].toBool());
    QJsonArray partA = result["partA"].toArray();
    QJsonArray partB = result["partB"].toArray();
    QVERIFY(partA.size() > 0);
    QVERIFY(partB.size() > 0);
    QCOMPARE(partA.size() + partB.size(), 4);
}

//недвудольный граф
void TestGraphFunctions::testTask3_NonBipartiteGraph()
{
    QJsonArray edges;
    edges.append(QJsonArray{0, 1});
    edges.append(QJsonArray{1, 2});
    edges.append(QJsonArray{2, 0});

    QJsonObject result = task3_BipartiteParts(edges, 3);
    QCOMPARE(result["status"].toString(), QString("error"));
    QCOMPARE(result["is_bipartite"].toBool(), false);
}

//Существует ли путь между двумя вершинами
void TestGraphFunctions::testTask4_PathExists()
{
    QJsonArray edges;
    edges.append(QJsonArray{0, 1});
    edges.append(QJsonArray{1, 2});

    QJsonObject result = task4_HasPath(edges, 3, 0, 2);
    QVERIFY(result["has_path"].toBool());
}

void TestGraphFunctions::testTask4_NoPathExists()
{
    QJsonArray edges;
    edges.append(QJsonArray{0, 1});
    edges.append(QJsonArray{2, 3});

    QJsonObject result = task4_HasPath(edges, 4, 0, 3);
    QVERIFY(!result["has_path"].toBool());
}

QTEST_APPLESS_MAIN(TestGraphFunctions)

#include "tst_testgraphfunctions.moc"
