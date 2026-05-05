#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <QVector>
#include <QPair>
#include <QPointF>

class GraphData
{
public:
    GraphData() = default;

    void setData(const QVector<QPointF> &vertices,
                 const QVector<QPair<qsizetype, qsizetype>> &edges);

    int vertexCount() const;
    QVector<qsizetype> getNeighbors(qsizetype vertexIndex) const;

private:
    QVector<QPointF> m_vertices;
    QVector<QPair<qsizetype, qsizetype>> m_edges;
};

#endif // GRAPHDATA_H