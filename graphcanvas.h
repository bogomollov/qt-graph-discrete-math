#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QPointF> // точка с координатами x,y
#include <QPair>   // пара значений (first, second)
#include <QVector> // динамический массив
#include <QWidget>
#include <QColor>

class GraphCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit GraphCanvas(QWidget *parent = nullptr);

    // Геттеры для получения данных
    QVector<QPointF> getVertices() const { return vertices; }
    QVector<QPair<qsizetype, qsizetype>> getEdges() const { return edges; }
    QVector<double> getEdgeWeights() const { return edgeWeights; }
    qsizetype getStartVertex() const { return startVertexIndex; }
    bool isDirected() const { return m_isDirected; }
    bool showEdgeWeights() const { return m_showEdgeWeights; }

    // Сеттеры для установки данных
    void setData(const QVector<QPointF> &newVertices,
                 const QVector<QPair<qsizetype, qsizetype>> &newEdges,
                 const QVector<double> &newWeights = {});
    void clear();

    // Методы подсветки
    void highlightVertex(int index, const QColor &color);
    void highlightEdge(int from, int to, const QColor &color);
    void clearHighlights();
    void setVertexColors(const QVector<QColor> &colors);
    void clearVertexColors();

    void deleteVertex(qsizetype index);

    // Методы для работы со стартовой вершиной
    void setStartVertex(qsizetype index);
    void clearStartVertex();

    void setDirected(bool directed);
    void setShowEdgeWeights(bool show);

signals:
    void graphChanged();  // сигнал: граф изменился

    // Обработчики событий
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVector<QPointF> vertices;
    QVector<QPair<qsizetype, qsizetype>> edges;
    QVector<double> edgeWeights;
    qsizetype selectedVertexIndex = -1;
    qsizetype startVertexIndex = -1;
    QVector<QColor> m_vertexColors;
    bool m_isDragging = false;
    qsizetype m_draggedVertexIndex = -1;
    bool m_isDirected = false;
    bool m_showEdgeWeights = true;
    bool m_allSelected = false;
    QPointF m_lastDragPos;

    void separateVertices(qsizetype pinned = -1);
    qsizetype edgeAt(const QPointF &pos) const;

    struct Highlight {
        bool isEdge = false;
        int v1 = -1;
        int v2 = -1;
        QColor color;
    };
    QVector<Highlight> m_highlights;
};

#endif // GRAPHCANVAS_H
