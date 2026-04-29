#include "graphcanvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QString>

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

    QFont vertexLabelFont = painter.font();
    vertexLabelFont.setBold(true);

    constexpr qreal vertexRadius = 24.0;
    for (qsizetype index = 0; index < vertices.size(); ++index) {
        const QPointF &vertex = vertices.at(index);
        const QRectF vertexRect(vertex.x() - vertexRadius,
                                vertex.y() - vertexRadius,
                                vertexRadius * 2.0,
                                vertexRadius * 2.0);

        painter.setPen(QPen(QColor(255, 174, 24), 2));
        painter.drawEllipse(vertex, vertexRadius, vertexRadius);
        painter.setPen(QColor(34, 34, 34));
        painter.setFont(vertexLabelFont);
        painter.drawText(vertexRect, Qt::AlignCenter, QString::number(index + 1));
    }
}
