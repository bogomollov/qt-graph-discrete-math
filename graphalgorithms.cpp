#include "graphalgorithms.h"
#include "graphdata.h"
#include <QQueue>
#include <QString>

QStringList GraphAlgorithms::bfs(const GraphData &graph, qsizetype startVertex)
{
    QStringList log;

    if (graph.vertexCount() == 0 || startVertex < 0 || startVertex >= graph.vertexCount()) {
        log << "Ошибка: граф пуст или неверная стартовая вершина";
        return log;
    }

    QVector<bool> visited(graph.vertexCount(), false);
    QQueue<qsizetype> queue;

    log << QString("=== BFS из вершины %1 ===").arg(startVertex + 1);

    visited[startVertex] = true;
    queue.enqueue(startVertex);
    log << QString("Шаг 1: Посетили вершину %1, добавили в очередь").arg(startVertex + 1);

    int step = 2;
    while (!queue.isEmpty()) {
        qsizetype current = queue.dequeue();
        log << QString("Шаг %1: Извлекли вершину %2 из очереди").arg(step++).arg(current + 1);

        QVector<qsizetype> neighbors = graph.getNeighbors(current);
        for (qsizetype neighbor : neighbors) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.enqueue(neighbor);
                log << QString("Шаг %1: Посетили вершину %2 (сосед %3), добавили в очередь")
                           .arg(step++)
                           .arg(neighbor + 1)
                           .arg(current + 1);
            }
        }
    }

    log << "=== BFS завершён ===";
    return log;
}

QStringList GraphAlgorithms::dfs(const GraphData &graph, qsizetype startVertex)
{
    QStringList log;

    if (graph.vertexCount() == 0 || startVertex < 0 || startVertex >= graph.vertexCount()) {
        log << "Ошибка: граф пуст или неверная стартовая вершина";
        return log;
    }

    QVector<bool> visited(graph.vertexCount(), false);
    QVector<qsizetype> stack;
    int step = 1;

    log << QString("=== DFS из вершины %1 ===").arg(startVertex + 1);

    stack.push_back(startVertex);

    while (!stack.isEmpty()) {
        qsizetype vertex = stack.last();
        stack.removeLast();

        if (!visited[vertex]) {
            visited[vertex] = true;
            log << QString("Шаг %1: Посетили вершину %2").arg(step++).arg(vertex + 1);

            QVector<qsizetype> neighbors = graph.getNeighbors(vertex);
            for (int i = neighbors.size() - 1; i >= 0; --i) {
                if (!visited[neighbors[i]]) {
                    stack.push_back(neighbors[i]);
                    log << QString("Шаг %1: Добавили вершину %2 в стек (сосед %3)")
                               .arg(step++)
                               .arg(neighbors[i] + 1)
                               .arg(vertex + 1);
                }
            }
        }
    }

    log << "=== DFS завершён ===";
    return log;
}