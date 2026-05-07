#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

#include <QVector>
#include <QPair>
#include <QString>

class GraphData;

// Один шаг алгоритма
struct AlgorithmStep {
    enum Type {
        VisitVertex,
        DiscoverEdge,
        Backtrack,
        Start,
        Finish
    };

    Type type;
    int vertex;        // основная вершина
    int secondVertex;  // вторая вершина (для ребра)
    QString description;
};

class GraphAlgorithms
{
public:
    GraphAlgorithms() = default;

    QVector<AlgorithmStep> bfs(const GraphData &graph, int startVertex);
    QVector<AlgorithmStep> dfs(const GraphData &graph, int startVertex);
    QVector<QPair<qsizetype, qsizetype>> spanningTree(const GraphData &graph,
                                                      bool minimum,
                                                      double *totalWeight = nullptr);
};

#endif
