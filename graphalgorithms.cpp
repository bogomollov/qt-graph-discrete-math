#include "graphalgorithms.h"
#include "graphdata.h"
#include <algorithm>
#include <cmath>
#include <limits>
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

bool canUseColor(const QVector<QVector<int>> &adjacency,
                 const QVector<int> &colors,
                 int vertex,
                 int color)
{
    for (int neighbor : adjacency[vertex]) {
        if (colors[neighbor] == color) {
            return false;
        }
    }
    return true;
}

bool colorByBacktracking(const QVector<QVector<int>> &adjacency,
                         const QVector<int> &order,
                         int position,
                         int colorLimit,
                         QVector<int> &colors)
{
    if (position == order.size()) {
        return true;
    }

    const int vertex = order[position];
    for (int color = 0; color < colorLimit; ++color) {
        if (!canUseColor(adjacency, colors, vertex, color)) {
            continue;
        }

        colors[vertex] = color;
        if (colorByBacktracking(adjacency, order, position + 1, colorLimit, colors)) {
            return true;
        }
        colors[vertex] = -1;
    }

    return false;
}
}

QVector<AlgorithmStep> GraphAlgorithms::bfs(const GraphData &graph, int startVertex)
{
    QVector<AlgorithmStep> steps;

    if (graph.vertexCount() == 0 || startVertex < 0 || startVertex >= graph.vertexCount()) {
        steps.append({AlgorithmStep::Start, -1, -1, "Ошибка: граф пуст или неверная вершина"});
        return steps;
    }

    steps.append({AlgorithmStep::Start, startVertex, -1,
                  QString("=== BFS из вершины %1 ===").arg(startVertex + 1)});

    QVector<bool> visited(graph.vertexCount(), false);
    QQueue<int> queue;

    // Visit all components: start from startVertex, then restart from any unvisited vertex
    for (int seed = 0; seed < graph.vertexCount(); ++seed) {
        const int origin = (seed == 0) ? startVertex : seed;
        if (visited[origin])
            continue;

        visited[origin] = true;
        queue.enqueue(origin);
        steps.append({AlgorithmStep::VisitVertex, origin, -1,
                      QString("Посетили вершину %1").arg(origin + 1)});

        while (!queue.isEmpty()) {
            int current = queue.dequeue();
            for (auto n : graph.getNeighbors(current)) {
                const int neighbor = static_cast<int>(n);
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
        weightedEdges.append({edge.first, edge.second, edgeDisplayWeight(std::hypot(delta.x(), delta.y()))});
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

QVector<int> GraphAlgorithms::greedyColoring(const GraphData &graph)
{
    const int vertexCount = graph.vertexCount();
    QVector<int> colors(vertexCount, -1);

    for (int vertex = 0; vertex < vertexCount; ++vertex) {
        QVector<bool> usedColors(vertexCount, false);

        for (qsizetype neighborIndex : graph.getNeighborsUndirected(vertex)) {
            const int neighbor = static_cast<int>(neighborIndex);
            if (neighbor >= 0 && neighbor < vertexCount && colors[neighbor] >= 0) {
                usedColors[colors[neighbor]] = true;
            }
        }

        int color = 0;
        while (color < usedColors.size() && usedColors[color]) {
            ++color;
        }

        colors[vertex] = color;
    }

    return colors;
}

QVector<int> GraphAlgorithms::backtrackingColoring(const GraphData &graph)
{
    const int vertexCount = graph.vertexCount();
    QVector<QVector<int>> adjacency(vertexCount);

    for (int vertex = 0; vertex < vertexCount; ++vertex) {
        for (qsizetype neighborIndex : graph.getNeighborsUndirected(vertex)) {
            const int neighbor = static_cast<int>(neighborIndex);
            if (neighbor >= 0 && neighbor < vertexCount && neighbor != vertex
                && !adjacency[vertex].contains(neighbor)) {
                adjacency[vertex].append(neighbor);
            }
        }
    }

    QVector<int> order;
    order.reserve(vertexCount);
    for (int vertex = 0; vertex < vertexCount; ++vertex) {
        order.append(vertex);
    }

    std::sort(order.begin(), order.end(),
              [&adjacency](int left, int right) {
                  if (adjacency[left].size() == adjacency[right].size()) {
                      return left < right;
                  }
                  return adjacency[left].size() > adjacency[right].size();
              });

    for (int colorLimit = 1; colorLimit <= vertexCount; ++colorLimit) {
        QVector<int> colors(vertexCount, -1);
        if (colorByBacktracking(adjacency, order, 0, colorLimit, colors)) {
            return colors;
        }
    }

    return QVector<int>(vertexCount, -1);
}

QVector<AlgorithmStep> GraphAlgorithms::dfs(const GraphData &graph, int startVertex)
{
    QVector<AlgorithmStep> steps;

    if (graph.vertexCount() == 0 || startVertex < 0 || startVertex >= graph.vertexCount()) {
        steps.append({AlgorithmStep::Start, -1, -1, "Ошибка: граф пуст или неверная вершина"});
        return steps;
    }

    steps.append({AlgorithmStep::Start, startVertex, -1,
                  QString("=== DFS из вершины %1 ===").arg(startVertex + 1)});

    QVector<bool> visited(graph.vertexCount(), false);
    QVector<int> stack;

    // Visit all components: start from startVertex, then restart from any unvisited vertex
    for (int seed = 0; seed < graph.vertexCount(); ++seed) {
        const int origin = (seed == 0) ? startVertex : seed;
        if (visited[origin])
            continue;

        stack.push_back(origin);

        while (!stack.isEmpty()) {
            int vertex = stack.last();
            stack.removeLast();

            if (visited[vertex])
                continue;

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

QVector<QPair<qsizetype, qsizetype>> GraphAlgorithms::dijkstra(const GraphData &graph,
                                                                int startVertex,
                                                                int endVertex,
                                                                double *totalWeight)
{
    if (totalWeight)
        *totalWeight = 0.0;

    const int n = graph.vertexCount();
    if (n == 0 || startVertex < 0 || startVertex >= n || endVertex < 0 || endVertex >= n)
        return {};

    const double inf = std::numeric_limits<double>::infinity();
    QVector<double> dist(n, inf);
    QVector<int> prev(n, -1);
    QVector<bool> visited(n, false);
    dist[startVertex] = 0.0;

    for (int iter = 0; iter < n; ++iter) {
        // Pick unvisited vertex with smallest distance
        int u = -1;
        for (int i = 0; i < n; ++i) {
            if (!visited[i] && (u < 0 || dist[i] < dist[u]))
                u = i;
        }
        if (u < 0 || dist[u] == inf)
            break;
        visited[u] = true;

        for (const auto &edge : graph.edges()) {
            int v = -1;
            if (static_cast<int>(edge.first) == u)
                v = static_cast<int>(edge.second);
            else if (!graph.isDirected() && static_cast<int>(edge.second) == u)
                v = static_cast<int>(edge.first);
            if (v < 0 || visited[v])
                continue;

            const QPointF delta = graph.vertices().at(u) - graph.vertices().at(v);
            const double w = edgeDisplayWeight(std::hypot(delta.x(), delta.y()));
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
            }
        }
    }

    if (dist[endVertex] == inf)
        return {};

    // Reconstruct path edges
    QVector<QPair<qsizetype, qsizetype>> path;
    for (int v = endVertex; prev[v] >= 0; v = prev[v])
        path.prepend(qMakePair(static_cast<qsizetype>(prev[v]), static_cast<qsizetype>(v)));

    if (totalWeight)
        *totalWeight = dist[endVertex];

    return path;
}
