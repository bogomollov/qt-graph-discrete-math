#include "graphdata.h"

void GraphData::setData(const QVector<QPointF> &vertices,
                        const QVector<QPair<qsizetype, qsizetype>> &edges,
                        const QVector<double> &weights)
{
    m_vertices = vertices;
    m_edges = edges;
    m_edgeWeights = weights;
    m_edgeWeights.resize(m_edges.size(), 1.0);
}

int GraphData::vertexCount() const
{
    return m_vertices.size();
}

const QVector<QPointF> &GraphData::vertices() const
{
    return m_vertices;
}

const QVector<QPair<qsizetype, qsizetype>> &GraphData::edges() const
{
    return m_edges;
}

const QVector<double> &GraphData::edgeWeights() const
{
    return m_edgeWeights;
}

QVector<qsizetype> GraphData::getNeighbors(qsizetype vertexIndex) const
{
    QVector<qsizetype> neighbors;

    for (const auto &edge : m_edges) {
        if (edge.first == vertexIndex) {
            neighbors.append(edge.second);
        } else if (!m_isDirected && edge.second == vertexIndex) {
            neighbors.append(edge.first);
        }
    }

    return neighbors;
}

QVector<qsizetype> GraphData::getNeighborsUndirected(qsizetype vertexIndex) const
{
    QVector<qsizetype> neighbors;

    for (const auto &edge : m_edges) {
        if (edge.first == vertexIndex) {
            neighbors.append(edge.second);
        } else if (edge.second == vertexIndex) {
            neighbors.append(edge.first);
        }
    }

    return neighbors;
}
