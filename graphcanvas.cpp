#include "graphcanvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QColor>
#include <QKeyEvent>

#include <utility>
#include <cmath>

namespace {
constexpr qreal vertexRadius = 24.0;
constexpr qreal vertexClickTolerance = 6.0;
constexpr qreal arrowSize = 10.0;
const QColor startVertexColor(255, 0, 0);

void drawArrowHead(QPainter &painter, const QPointF &from, const QPointF &to)
{
    const QPointF delta = to - from;
    const qreal length = std::hypot(delta.x(), delta.y());
    if (length < 1e-6)
        return;

    // Unit vector along the edge
    const QPointF unit = delta / length;
    // Tip of arrowhead sits on the circle boundary of the target vertex
    const QPointF tip = to - unit * vertexRadius;

    // Two base points of the arrowhead triangle
    const QPointF perp(-unit.y(), unit.x());
    const QPointF base = tip - unit * arrowSize;
    const QPointF p1 = base + perp * (arrowSize * 0.5);
    const QPointF p2 = base - perp * (arrowSize * 0.5);

    const QPolygonF arrow = {tip, p1, p2};
    painter.drawPolygon(arrow);
}
}

GraphCanvas::GraphCanvas(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
    setMinimumSize(640, 480);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
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

void GraphCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete && selectedVertexIndex >= 0) {
        const qsizetype removedIndex = selectedVertexIndex;
        vertices.removeAt(removedIndex);
        QVector<QPair<qsizetype, qsizetype>> newEdges;
        newEdges.reserve(edges.size());

        for (const auto &edge : edges) {
            qsizetype from = edge.first;
            qsizetype to = edge.second;

            if (from == removedIndex || to == removedIndex)
                continue;
            if (from > removedIndex) --from;
            if (to > removedIndex) --to;

            newEdges.append(qMakePair(from, to));
        }
        edges = std::move(newEdges);
        selectedVertexIndex = -1;
        m_highlights.clear();
        m_vertexColors.clear();
        update();
        emit graphChanged();
        return;
    }

    QWidget::keyPressEvent(event);
}

void GraphCanvas::mousePressEvent(QMouseEvent *event)
{
    setFocus();
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
            m_vertexColors.clear();
            emit graphChanged();
        } else {
            selectedVertexIndex = clickedVertexIndex;
        }

        update();
        return;
    }

    vertices.append(clickPosition);
    selectedVertexIndex = -1;
    m_vertexColors.clear();
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

    // Рисуем рёбра
    painter.setPen(QPen(QColor(120, 120, 120), 2));
    for (const QPair<qsizetype, qsizetype> &edge : std::as_const(edges)) {
        painter.drawLine(vertices.at(edge.first), vertices.at(edge.second));
        if (m_isDirected) {
            painter.setBrush(QColor(120, 120, 120));
            painter.setPen(Qt::NoPen);
            drawArrowHead(painter, vertices.at(edge.first), vertices.at(edge.second));
            painter.setPen(QPen(QColor(120, 120, 120), 2));
            painter.setBrush(Qt::NoBrush);
        }
    }

    // Отрисовка подсветки
    for (const auto &hl : m_highlights) {
        if (hl.isEdge) {
            QPen pen(hl.color, 3);
            painter.setPen(pen);
            painter.drawLine(vertices.at(hl.v1), vertices.at(hl.v2));
            if (m_isDirected) {
                painter.setBrush(hl.color);
                painter.setPen(Qt::NoPen);
                drawArrowHead(painter, vertices.at(hl.v1), vertices.at(hl.v2));
                painter.setPen(pen);
                painter.setBrush(Qt::NoBrush);
            }
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

    // Рисуем вершины
    for (qsizetype index = 0; index < vertices.size(); ++index) {
        const QPointF &vertex = vertices.at(index);
        const QRectF vertexRect(vertex.x() - vertexRadius,
                                vertex.y() - vertexRadius,
                                vertexRadius * 2.0,
                                vertexRadius * 2.0);

        // Определяем цвет обводки
        QColor outlineColor;
        if (index == startVertexIndex) {
            outlineColor = startVertexColor;  // Фиолетовый для стартовой
        } else if (index == selectedVertexIndex) {
            outlineColor = QColor(214, 130, 0);  // Оранжевый для выбранной
        } else {
            outlineColor = QColor(255, 174, 24); // Обычный цвет
        }

        painter.setPen(QPen(outlineColor, 2));
        if (index < m_vertexColors.size() && m_vertexColors.at(index).isValid()) {
            painter.setBrush(m_vertexColors.at(index));
        } else {
            painter.setBrush(QColor(255, 205, 112));
        }
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

void GraphCanvas::setVertexColors(const QVector<QColor> &colors)
{
    m_vertexColors = colors;
    m_vertexColors.resize(vertices.size());
    update();
}

void GraphCanvas::clearVertexColors()
{
    m_vertexColors.clear();
    update();
}

void GraphCanvas::setData(const QVector<QPointF> &newVertices,
                          const QVector<QPair<qsizetype, qsizetype>> &newEdges)
{
    vertices = newVertices;
    edges = newEdges;
    selectedVertexIndex = -1;
    startVertexIndex = -1;
    m_highlights.clear();
    m_vertexColors.clear();
    update();
    emit graphChanged();
}

void GraphCanvas::clear()
{
    vertices.clear();
    edges.clear();
    selectedVertexIndex = -1;
    startVertexIndex = -1;
    m_highlights.clear();
    m_vertexColors.clear();
    update();
    emit graphChanged();
}

void GraphCanvas::deleteVertex(qsizetype index)
{
    if (index < 0 || index >= vertices.size())
        return;

    // Удаляем вершину
    vertices.remove(index);

    // Обновляем индексы в рёбрах
    QVector<QPair<qsizetype, qsizetype>> newEdges;
    for (const auto &edge : edges) {
        qsizetype from = edge.first;
        qsizetype to = edge.second;

        // Если ребро не связано с удаляемой вершиной
        if (from != index && to != index) {
            // Корректируем индексы (сдвигаем влево те, что больше удалённой)
            if (from > index) from--;
            if (to > index) to--;
            newEdges.append(qMakePair(from, to));
        }
        // Ребро, связанное с удаляемой вершиной, просто пропускаем (удаляется)
    }

    edges = newEdges;

    // Снимаем выделение
    selectedVertexIndex = -1;

    // Очищаем подсветку
    m_highlights.clear();
    m_vertexColors.clear();

    update();
    emit graphChanged();
}

// --- Работа со стартовой вершиной ---
void GraphCanvas::setStartVertex(qsizetype index)
{
    if (index >= 0 && index < vertices.size()) {
        startVertexIndex = index;
    } else {
        startVertexIndex = -1;
    }
    update(); // Перерисовываем
}

void GraphCanvas::clearStartVertex()
{
    startVertexIndex = -1;
    update();
}

void GraphCanvas::setDirected(bool directed)
{
    m_isDirected = directed;
    update();
}
