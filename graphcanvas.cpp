#include "graphcanvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QColor>

#include <utility>

namespace {
constexpr qreal vertexRadius = 24.0; // Радиус вершины
constexpr qreal vertexClickTolerance = 6.0;
}

GraphCanvas::GraphCanvas(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
    setMinimumSize(640, 480);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
}

void GraphCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_draggedVertexIndex = -1;
    }

    QWidget::mouseReleaseEvent(event);
}

void GraphCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_isDragging || m_draggedVertexIndex < 0)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPointF pos = event->position();
#else
    const QPointF pos = event->pos();
#endif

    vertices[m_draggedVertexIndex] = pos;

    update();
    emit graphChanged();
}

void GraphCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPointF clickPosition = event->position();
#else
    const QPointF clickPosition = event->pos();
#endif

    qsizetype clickedVertexIndex = -1;
    qreal closestDistanceSquared = 0.0;
    const qreal hitRadius = vertexRadius + vertexClickTolerance;
    const qreal hitRadiusSquared = hitRadius * hitRadius;

    for (qsizetype index = 0; index < vertices.size(); ++index) {
        const QPointF distance = vertices.at(index) - clickPosition;
        const qreal distanceSquared = QPointF::dotProduct(distance, distance);
        if (distanceSquared <= hitRadiusSquared
                && (clickedVertexIndex < 0 || distanceSquared < closestDistanceSquared)) {
            clickedVertexIndex = index;
            closestDistanceSquared = distanceSquared;
        }
    }

    if (clickedVertexIndex >= 0) {
        m_isDragging = true;
        m_draggedVertexIndex = clickedVertexIndex;

        if (selectedVertexIndex >= 0 && selectedVertexIndex != clickedVertexIndex) {
            edges.append(qMakePair(selectedVertexIndex, clickedVertexIndex));
            selectedVertexIndex = -1;
            emit graphChanged();
        } else {
            selectedVertexIndex = clickedVertexIndex;
        }

        update();
        return;
    }

    vertices.append(clickPosition);
    selectedVertexIndex = -1;
    emit graphChanged();
    update();
}

void GraphCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    painter.setPen(QPen(QColor(255, 174, 24), 2));
    painter.setBrush(QColor(255, 205, 112));

    painter.setPen(QPen(QColor(120, 120, 120), 2));
    for (const QPair<qsizetype, qsizetype> &edge : std::as_const(edges)) {
        painter.drawLine(vertices.at(edge.first), vertices.at(edge.second));
    }

    // Отрисовка подсветки
    for (const auto &hl : m_highlights) {
        if (hl.isEdge) {
            QPen pen(hl.color, 3);
            painter.setPen(pen);
            painter.drawLine(vertices.at(hl.v1), vertices.at(hl.v2));
        } else {
            QPen pen(hl.color, 3);
            painter.setPen(pen);
            painter.setBrush(hl.color.lighter(150));
            const QPointF &v = vertices.at(hl.v1);
            painter.drawEllipse(v, vertexRadius + 4, vertexRadius + 4);
        }
    }

    QFont vertexLabelFont = painter.font();
    vertexLabelFont.setBold(true);

    for (qsizetype index = 0; index < vertices.size(); ++index) {
        const QPointF &vertex = vertices.at(index);
        const QRectF vertexRect(vertex.x() - vertexRadius,
                                vertex.y() - vertexRadius,
                                vertexRadius * 2.0,
                                vertexRadius * 2.0);

        const QColor outlineColor = index == selectedVertexIndex
                                        ? QColor(214, 130, 0)
                                        : QColor(255, 174, 24);
        painter.setPen(QPen(outlineColor, 2));
        painter.drawEllipse(vertex, vertexRadius, vertexRadius);
        painter.setPen(QColor(34, 34, 34));
        painter.setFont(vertexLabelFont);
        painter.drawText(vertexRect, Qt::AlignCenter, QString::number(index + 1));
    }
}


// Реализация подсветки
void GraphCanvas::highlightVertex(int index, const QColor &color)
{
    if (index < 0 || index >= vertices.size()) return;
    m_highlights.append({false, index, -1, color});
    update();
}

void GraphCanvas::highlightEdge(int from, int to, const QColor &color)
{
    m_highlights.append({true, from, to, color});
    update();
}

void GraphCanvas::clearHighlights()
{
    m_highlights.clear();
    update();
}

void GraphCanvas::setData(const QVector<QPointF> &newVertices,
                          const QVector<QPair<qsizetype, qsizetype>> &newEdges)
{
    vertices = newVertices;
    edges = newEdges;
    selectedVertexIndex = -1;
    m_highlights.clear();
    update();
    emit graphChanged();
}

void GraphCanvas::clear()
{
    vertices.clear();
    edges.clear();
    selectedVertexIndex = -1;
    m_highlights.clear();
    update();
    emit graphChanged();
}
