#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QPointF> // точка с координатами x,y
#include <QPair>   // пара значений (first, second)
#include <QVector> // динамический массив
#include <QWidget>

class GraphCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit GraphCanvas(QWidget *parent = nullptr);

    QVector<QPointF> getVertices() const { return vertices; }
    QVector<QPair<qsizetype, qsizetype>> getEdges() const { return edges; }

    void setData(const QVector<QPointF> &newVertices,
                 const QVector<QPair<qsizetype, qsizetype>> &newEdges);
    void clear();

    // Методы подсветки
    void highlightVertex(int index, const QColor &color);
    void highlightEdge(int from, int to, const QColor &color);
    void clearHighlights();

    void deleteVertex(qsizetype index);

    // Методы для работы со стартовой вершиной
    void setStartVertex(qsizetype index);
    qsizetype getStartVertex() const { return startVertexIndex; }
    void clearStartVertex();

signals:
    void graphChanged();  // сигнал: граф изменился

// Обработчики событий
protected:
    void mousePressEvent(QMouseEvent *event) override; // Клик мышью
    void paintEvent(QPaintEvent *event) override;      // Вызывается когда нужно перерисовать виджет
    void keyPressEvent(QKeyEvent *event) override;     // Нажатие клавиш

private:
    QVector<QPointF> vertices;                   // Координаты всех вершин
    QVector<QPair<qsizetype, qsizetype>> edges;  // Связи (пара индексов)
    qsizetype selectedVertexIndex = -1;          // Выбранная вершина ( -1 ничего )
    qsizetype startVertexIndex = -1;             // Стартовая вершина

    struct Highlight {
        bool isEdge = false;
        int v1 = -1;
        int v2 = -1;
        QColor color;
    };
    QVector<Highlight> m_highlights;
};

#endif // GRAPHCANVAS_H
