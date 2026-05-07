#include "graphalgorithms.h"
#include "graphdata.h"
#include <algorithm>
#include <cmath>
#include <QQueue>
#include <QVector>

namespace {
class DisjointSet
{
public:
    explicit DisjointSet(int size)
        : parent(size)
        , rank(size, 0)
    {
        for (int i = 0; i < size; ++i) {
            parent[i] = i;
        }
    }

    int find(int value)
    {
        if (parent[value] != value) {
            parent[value] = find(parent[value]);
        }
        return parent[value];
    }

    bool unite(int left, int right)
    {
        int leftRoot = find(left);
        int rightRoot = find(right);

        if (leftRoot == rightRoot) {
            return false;
        }

        if (rank[leftRoot] < rank[rightRoot]) {
            std::swap(leftRoot, rightRoot);
        }

        parent[rightRoot] = leftRoot;
        if (rank[leftRoot] == rank[rightRoot]) {
            ++rank[leftRoot];
        }

        return true;
    }

private:
    QVector<int> parent;
    QVector<int> rank;
};

struct WeightedEdge {
    qsizetype from = -1;
    qsizetype to = -1;
    double weight = 0.0;
};
}

QVector<AlgorithmStep> GraphAlgorithms::bfs(const GraphData &graph, int startVertex)
{
    QVector<AlgorithmStep> steps;

    if (graph.vertexCount() == 0 || startVertex < 0 || startVertex >= graph.vertexCount()) {
        steps.append({AlgorithmStep::Start, -1, -1, "Ошибка: граф пуст или неверная вершина"});
        return steps;
    }

    QVector<bool> visited(graph.vertexCount(), false);
    QQueue<int> queue;

    steps.append({AlgorithmStep::Start, startVertex, -1,
                  QString("=== BFS из вершины %1 ===").arg(startVertex + 1)});

    visited[startVertex] = true;
    queue.enqueue(startVertex);
    steps.append({AlgorithmStep::VisitVertex, startVertex, -1,
                  QString("Посетили вершину %1").arg(startVertex + 1)});

    while (!queue.isEmpty()) {
        int current = queue.dequeue();

        QVector<int> neighbors;
        // конвертируем qsizetype -> int
        for (auto n : graph.getNeighbors(current))
            neighbors.append(static_cast<int>(n));

        for (int neighbor : neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.enqueue(neighbor);

                steps.append({AlgorithmStep::DiscoverEdge, current, neighbor,
                              QString("Ребро %1-%2").arg(current + 1).arg(neighbor + 1)});
                steps.append({AlgorithmStep::VisitVertex, neighbor, -1,
                              QString("Посетили вершину %1").arg(neighbor + 1)});
            }
        }
    }

    steps.append({AlgorithmStep::Finish, -1, -1, "=== BFS завершён ==="});
    return steps;
}

QVector<QPair<qsizetype, qsizetype>> GraphAlgorithms::spanningTree(const GraphData &graph,
                                                                   bool minimum,
                                                                   double *totalWeight)
{
    if (totalWeight) {
        *totalWeight = 0.0;
    }

    QVector<QPair<qsizetype, qsizetype>> treeEdges;
    const int vertexCount = graph.vertexCount();
    if (vertexCount <= 1) {
        return treeEdges;
    }

    QVector<WeightedEdge> weightedEdges;
    weightedEdges.reserve(graph.edges().size());

    for (const auto &edge : graph.edges()) {
        if (edge.first == edge.second
            || edge.first < 0 || edge.second < 0
            || edge.first >= vertexCount || edge.second >= vertexCount) {
            continue;
        }

        const QPointF delta = graph.vertices().at(edge.first) - graph.vertices().at(edge.second);
        weightedEdges.append({edge.first, edge.second, std::hypot(delta.x(), delta.y())});
    }

    std::sort(weightedEdges.begin(), weightedEdges.end(),
              [minimum](const WeightedEdge &left, const WeightedEdge &right) {
                  if (left.weight == right.weight) {
                      if (left.from == right.from) {
                          return left.to < right.to;
                      }
                      return left.from < right.from;
                  }
                  return minimum ? left.weight < right.weight : left.weight > right.weight;
              });

    DisjointSet components(vertexCount);
    treeEdges.reserve(vertexCount - 1);

    for (const WeightedEdge &edge : weightedEdges) {
        if (!components.unite(static_cast<int>(edge.from), static_cast<int>(edge.to))) {
            continue;
        }

        treeEdges.append(qMakePair(edge.from, edge.to));
        if (totalWeight) {
            *totalWeight += edge.weight;
        }

        if (treeEdges.size() == vertexCount - 1) {
            break;
        }
    }

    return treeEdges;
}

QVector<AlgorithmStep> GraphAlgorithms::dfs(const GraphData &graph, int startVertex)
{
    QVector<AlgorithmStep> steps;

    if (graph.vertexCount() == 0 || startVertex < 0 || startVertex >= graph.vertexCount()) {
        steps.append({AlgorithmStep::Start, -1, -1, "Ошибка: граф пуст или неверная вершина"});
        return steps;
    }

    QVector<bool> visited(graph.vertexCount(), false);
    QVector<int> stack;

    steps.append({AlgorithmStep::Start, startVertex, -1,
                  QString("=== DFS из вершины %1 ===").arg(startVertex + 1)});

    stack.push_back(startVertex);

    while (!stack.isEmpty()) {
        int vertex = stack.last();
        stack.removeLast();

        if (!visited[vertex]) {
            visited[vertex] = true;
            steps.append({AlgorithmStep::VisitVertex, vertex, -1,
                          QString("Посетили вершину %1").arg(vertex + 1)});

            QVector<qsizetype> neighbors = graph.getNeighbors(vertex);

            for (int i = neighbors.size() - 1; i >= 0; --i) {
                int neighbor = static_cast<int>(neighbors[i]);
                if (!visited[neighbor]) {
                    stack.push_back(neighbor);
                    steps.append({AlgorithmStep::DiscoverEdge, vertex, neighbor,
                                  QString("Ребро %1-%2").arg(vertex + 1).arg(neighbor + 1)});
                }
            }
        }
    }

    steps.append({AlgorithmStep::Finish, -1, -1, "=== DFS завершён ==="});
    return steps;
}
