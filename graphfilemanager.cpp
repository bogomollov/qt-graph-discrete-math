#include "graphfilemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIODevice>

bool GraphFileManager::saveToFile(const QString &fileName,
                                  const QVector<QPointF> &vertices,
                                  const QVector<QPair<qsizetype, qsizetype>> &edges,
                                  bool directed,
                                  QString *errorMessage)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMessage) {
            *errorMessage = QString("Не удалось открыть файл для записи:\n%1\nОшибка: %2")
                                .arg(fileName, file.errorString());
        }
        return false;
    }

    // Создаем JSON объект для вершин
    QJsonArray verticesArray;
    for (const QPointF &vertex : vertices) {
        QJsonObject vertexObj;
        vertexObj["x"] = vertex.x();
        vertexObj["y"] = vertex.y();
        verticesArray.append(vertexObj);
    }

    // Создаем JSON объект для ребер
    QJsonArray edgesArray;
    for (const auto &edge : edges) {
        QJsonObject edgeObj;
        edgeObj["from"] = static_cast<int>(edge.first);
        edgeObj["to"] = static_cast<int>(edge.second);
        edgesArray.append(edgeObj);
    }

    // Создаем корневой объект
    QJsonObject rootObj;
    rootObj["version"] = "1.0";
    rootObj["directed"] = directed;
    rootObj["vertices"] = verticesArray;
    rootObj["edges"] = edgesArray;

    // Записываем в файл
    QJsonDocument doc(rootObj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

bool GraphFileManager::loadFromFile(const QString &fileName,
                                    QVector<QPointF> &vertices,
                                    QVector<QPair<qsizetype, qsizetype>> &edges,
                                    bool *directed,
                                    QString *errorMessage)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage) {
            *errorMessage = QString("Не удалось открыть файл для чтения:\n%1\nОшибка: %2")
                                .arg(fileName, file.errorString());
        }
        return false;
    }

    // Читаем содержимое файла
    QByteArray data = file.readAll();
    file.close();

    // Парсим JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        if (errorMessage) {
            *errorMessage = QString("Ошибка парсинга JSON:\n%1").arg(parseError.errorString());
        }
        return false;
    }

    if (!doc.isObject()) {
        if (errorMessage) {
            *errorMessage = "Некорректный формат файла: ожидается JSON объект";
        }
        return false;
    }

    QJsonObject rootObj = doc.object();

    // Проверяем версию (опционально)
    QString version = rootObj["version"].toString();
    if (version.isEmpty()) {
        if (errorMessage) {
            *errorMessage = "В файле отсутствует информация о версии";
        }
        return false;
    }

    if (directed) {
        *directed = rootObj["directed"].toBool(false);
    }

    // Загружаем вершины
    QJsonArray verticesArray = rootObj["vertices"].toArray();
    vertices.clear();
    vertices.reserve(verticesArray.size());

    for (const QJsonValue &value : verticesArray) {
        QJsonObject vertexObj = value.toObject();

        if (!vertexObj.contains("x") || !vertexObj.contains("y")) {
            if (errorMessage) {
                *errorMessage = "Некорректный формат вершины: отсутствуют координаты";
            }
            return false;
        }

        double x = vertexObj["x"].toDouble();
        double y = vertexObj["y"].toDouble();
        vertices.append(QPointF(x, y));
    }

    // Загружаем ребра
    QJsonArray edgesArray = rootObj["edges"].toArray();
    edges.clear();
    edges.reserve(edgesArray.size());

    for (const QJsonValue &value : edgesArray) {
        QJsonObject edgeObj = value.toObject();

        if (!edgeObj.contains("from") || !edgeObj.contains("to")) {
            if (errorMessage) {
                *errorMessage = "Некорректный формат ребра: отсутствуют индексы вершин";
            }
            return false;
        }

        const int fromInt = edgeObj["from"].toInt(-1);
        const int toInt = edgeObj["to"].toInt(-1);

        // Проверяем валидность индексов
        if (fromInt < 0 || fromInt >= vertices.size() || toInt < 0 || toInt >= vertices.size()) {
            if (errorMessage) {
                *errorMessage = QString("Некорректное ребро: вершины %1-%2 не существуют")
                                    .arg(fromInt + 1).arg(toInt + 1);
            }
            return false;
        }

        edges.append(qMakePair(static_cast<qsizetype>(fromInt), static_cast<qsizetype>(toInt)));
    }

    return true;
}