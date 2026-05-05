#include "graphalgorithms.h"
#include "graphdata.h"
#include <QQueue>
#include <QVector>

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