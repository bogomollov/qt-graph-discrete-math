#include "graphcanvas.h"

#include <QMouseEvent>
#include <QPainter>

GraphCanvas::GraphCanvas(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
    setMinimumSize(640, 480);
    setCursor(Qt::CrossCursor);
}

void GraphCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    vertices.append(event->position());
#else
    vertices.append(event->pos());
#endif
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

    constexpr qreal vertexRadius = 24.0;
    for (const QPointF &vertex : std::as_const(vertices)) {
        painter.drawEllipse(vertex, vertexRadius, vertexRadius);
    }
}
