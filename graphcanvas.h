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

signals:
    void graphChanged();  // сигнал: граф изменился

// Обработчики событий
protected:
    void mousePressEvent(QMouseEvent *event) override; // Клик мышью
    void paintEvent(QPaintEvent *event) override;      // Вызывается когда нужно перерисовать виджет

private:
    QVector<QPointF> vertices;                   // Координаты всех вершин
    QVector<QPair<qsizetype, qsizetype>> edges;  // Связи (пара индексов)
    qsizetype selectedVertexIndex = -1;          // Выбранная вершина ( -1 ничего )
};

#endif // GRAPHCANVAS_H