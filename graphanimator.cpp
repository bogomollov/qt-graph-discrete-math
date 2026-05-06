#include "graphanimator.h"
#include "graphcanvas.h"
#include <QColor>

namespace {
constexpr int animationIntervalMs = 600;        // Значение по умолчанию
const QColor visitedVertexColor(76, 175, 80);   // Зелёный для посещённой вершины
const QColor discoveredEdgeColor(33, 150, 243); // Синий для обнаруженного ребра
}

GraphAnimator::GraphAnimator(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setInterval(animationIntervalMs);
    connect(m_timer, &QTimer::timeout, this, &GraphAnimator::nextStep);
}

void GraphAnimator::setCanvas(GraphCanvas *canvas) { m_canvas = canvas; }

// Метод для установки скорости
void GraphAnimator::setAnimationSpeed(int milliseconds)
{
    if (milliseconds >= 50 && milliseconds <= 5000) {
        m_timer->setInterval(milliseconds);
    }
}

void GraphAnimator::start(const QVector<AlgorithmStep> &steps)
{
    if (!m_canvas || steps.isEmpty()) return;

    m_steps = steps;
    m_currentStep = 0;
    m_canvas->clearHighlights();
    m_timer->start();
}

void GraphAnimator::stop()
{
    m_timer->stop();
    m_steps.clear();
    m_currentStep = 0;
    if (m_canvas) {
        m_canvas->clearHighlights();
    }
    emit finished();
}

bool GraphAnimator::isRunning() const { return m_timer->isActive(); }

void GraphAnimator::nextStep()
{
    if (m_currentStep >= m_steps.size()) {
        stop();
        return;
    }

    const AlgorithmStep &step = m_steps[m_currentStep];

    switch (step.type) {
    case AlgorithmStep::Start:
        m_canvas->clearHighlights();
        break;

    case AlgorithmStep::VisitVertex:
        m_canvas->highlightVertex(step.vertex, visitedVertexColor);
        break;

    case AlgorithmStep::DiscoverEdge:
        m_canvas->highlightEdge(step.vertex, step.secondVertex, discoveredEdgeColor);
        break;

    case AlgorithmStep::Finish:
        m_canvas->clearHighlights();
        break;

    default:
        break;
    }

    m_currentStep++;
}