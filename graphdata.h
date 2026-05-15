#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#include <QVector>
#include <QPair>
#include <QPointF>
#include <algorithm>

constexpr double kMinVertexDistance = 62.0;

class GraphData
{
public:
    GraphData() = default;

    void setData(const QVector<QPointF> &vertices,
                 const QVector<QPair<qsizetype, qsizetype>> &edges,
                 const QVector<double> &weights = {});

    int vertexCount() const;
    QVector<qsizetype> getNeighbors(qsizetype vertexIndex) const;
    const QVector<QPointF> &vertices() const;
    const QVector<QPair<qsizetype, qsizetype>> &edges() const;
    const QVector<double> &edgeWeights() const;

    bool isDirected() const { return m_isDirected; }
    void setDirected(bool directed) { m_isDirected = directed; }
    QVector<qsizetype> getNeighborsUndirected(qsizetype vertexIndex) const;

private:
    QVector<QPointF> m_vertices;
    QVector<QPair<qsizetype, qsizetype>> m_edges;
    QVector<double> m_edgeWeights;
    bool m_isDirected = false;
};

#endif // GRAPHDATA_H
