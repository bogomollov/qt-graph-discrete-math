#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QPointF>
#include <QPair>
#include <QVector>
#include <QWidget>

class GraphCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit GraphCanvas(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPointF> vertices;
    QVector<QPair<qsizetype, qsizetype>> edges;
    qsizetype selectedVertexIndex = -1;
};

#endif // GRAPHCANVAS_H
