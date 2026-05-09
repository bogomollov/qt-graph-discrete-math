#ifndef GRAPHFILEMANAGER_H
#define GRAPHFILEMANAGER_H

#include <QString>
#include <QVector>
#include <QPair>
#include <QPointF>

class GraphFileManager
{
public:
    GraphFileManager() = default;

    // Сохранение графа в JSON файл
    static bool saveToFile(const QString &fileName,
                           const QVector<QPointF> &vertices,
                           const QVector<QPair<qsizetype, qsizetype>> &edges,
                           bool directed = false,
                           QString *errorMessage = nullptr);

    // Загрузка графа из JSON файла
    static bool loadFromFile(const QString &fileName,
                             QVector<QPointF> &vertices,
                             QVector<QPair<qsizetype, qsizetype>> &edges,
                             bool *directed = nullptr,
                             QString *errorMessage = nullptr);
};

#endif // GRAPHFILEMANAGER_H