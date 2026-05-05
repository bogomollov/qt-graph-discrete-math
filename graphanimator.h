#ifndef GRAPHANIMATOR_H
#define GRAPHANIMATOR_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include "graphalgorithms.h"

class GraphCanvas;

class GraphAnimator : public QObject
{
    Q_OBJECT

public:
    explicit GraphAnimator(QObject *parent = nullptr);

    void setCanvas(GraphCanvas *canvas);
    void start(const QVector<AlgorithmStep> &steps);
    void stop();
    bool isRunning() const;

signals:
    void logMessage(const QString &message);
    void finished();

private slots:
    void nextStep();

private:
    GraphCanvas *m_canvas = nullptr;
    QTimer *m_timer;
    QVector<AlgorithmStep> m_steps;
    int m_currentStep = 0;
};

#endif