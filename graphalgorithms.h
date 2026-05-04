#ifndef GRAPHALGORITHMS_H
#define GRAPHALGORITHMS_H

#include <QString>
#include <QVector>

class GraphData;

class GraphAlgorithms
{
public:
    GraphAlgorithms() = default;

    QStringList bfs(const GraphData &graph, qsizetype startVertex);
    QStringList dfs(const GraphData &graph, qsizetype startVertex);
};

#endif // GRAPHALGORITHMS_H