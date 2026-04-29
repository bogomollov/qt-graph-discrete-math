#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QPointF>
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
    qsizetype selectedVertexIndex = -1;
};

#endif // GRAPHCANVAS_H
