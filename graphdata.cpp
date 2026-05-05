#include "graphdata.h"

void GraphData::setData(const QVector<QPointF> &vertices,
                        const QVector<QPair<qsizetype, qsizetype>> &edges)
{
    m_vertices = vertices;
    m_edges = edges;
}

int GraphData::vertexCount() const
{
    return m_vertices.size();
}

QVector<qsizetype> GraphData::getNeighbors(qsizetype vertexIndex) const
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