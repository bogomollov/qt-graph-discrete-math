#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "graphcanvas.h"
#include "graphdata.h"
#include "graphalgorithms.h"
#include "graphanimator.h"
#include "graphfilemanager.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QColor>

// --- Конструктор ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphCanvas(new GraphCanvas(this))
    , graphData(new GraphData)
    , algorithms(new GraphAlgorithms)
    , animator(new GraphAnimator(this))
{
    ui->setupUi(this);
    ui->animationSpeedCombo->setCurrentText("600"); // Базовая задержка
    setWindowTitle(tr("qt-graph-discrete-math"));

    // Холст в ScrollArea
    graphCanvas->setMinimumSize(2000, 2000);
    ui->scrollArea->setWidget(graphCanvas);

    // Лог только для чтения
    ui->logOutput->setReadOnly(true);

    // При изменении графа обновляем данные
    connect(graphCanvas, &GraphCanvas::graphChanged, [this]() {
        graphData->setData(graphCanvas->getVertices(),
                           graphCanvas->getEdges());
    });

    animator->setCanvas(graphCanvas);
    connect(animator, &GraphAnimator::logMessage, ui->logOutput, &QPlainTextEdit::appendPlainText);
    connect(animator, &GraphAnimator::finished, [this]() {
        ui->bfsButton->setText("BFS");
        ui->dfsButton->setText("DFS");
        ui->bfsButton->setEnabled(true);
        ui->dfsButton->setEnabled(true);
    });

    // Подключаем изменение скорости анимации
    connect(ui->animationSpeedCombo, &QComboBox::currentTextChanged, [this](const QString &text) {
        int speedValue = text.toInt();
        int delay = 1200 - speedValue;
        animator->setAnimationSpeed(delay);
    });

    // Устанавливаем начальную скорость
    int initialSpeedValue = ui->animationSpeedCombo->currentText().toInt();
    animator->setAnimationSpeed(1200 - initialSpeedValue);
}

// --- Деструктор ---
MainWindow::~MainWindow()
{
    delete ui;
    delete graphData;
    delete algorithms;
}

// --- Обработчик кнопки BFS ---
void MainWindow::on_bfsButton_clicked()
{
    graphData->setData(graphCanvas->getVertices(), graphCanvas->getEdges());

    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    int startVertex = ui->startVertexSpin->value() - 1;

    // Устанавливаем стартовую вершину на холсте (фиолетовая подсветка)
    graphCanvas->setStartVertex(startVertex);

    if (animator->isRunning()) {
        animator->stop();
        return;
    }

    if (animator->isRunning()) {
        animator->stop();
        return;
    }

    auto steps = algorithms->bfs(*graphData, startVertex);

    QStringList order;
    for (const auto &step : steps) {
        if (step.type == AlgorithmStep::VisitVertex)
            order << QString::number(step.vertex + 1);
    }
    ui->logOutput->appendPlainText("BFS: " + order.join(" → "));

    ui->bfsButton->setText("СТОП");
    ui->dfsButton->setEnabled(false);
    animator->start(steps);
}

// --- Обработчик кнопки DFS ---
void MainWindow::on_dfsButton_clicked()
{
    graphData->setData(graphCanvas->getVertices(), graphCanvas->getEdges());

    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    int startVertex = ui->startVertexSpin->value() - 1;

    // Устанавливаем стартовую вершину на холсте (фиолетовая подсветка)
    graphCanvas->setStartVertex(startVertex);

    if (animator->isRunning()) {
        animator->stop();
        return;
    }

    if (animator->isRunning()) {
        animator->stop();
        return;
    }

    auto steps = algorithms->dfs(*graphData, startVertex);

    QStringList order;
    for (const auto &step : steps) {
        if (step.type == AlgorithmStep::VisitVertex)
            order << QString::number(step.vertex + 1);
    }
    ui->logOutput->appendPlainText("DFS: " + order.join(" → "));

    ui->dfsButton->setText("СТОП");
    ui->bfsButton->setEnabled(false);
    animator->start(steps);
}

// --- Меню Файл ---
void MainWindow::on_minSpanningTreeButton_clicked()
{
    buildSpanningTree(true);
}

void MainWindow::on_maxSpanningTreeButton_clicked()
{
    buildSpanningTree(false);
}

void MainWindow::buildSpanningTree(bool minimum)
{
    if (animator->isRunning()) {
        animator->stop();
    }

    const QVector<QPointF> vertices = graphCanvas->getVertices();
    graphData->setData(vertices, graphCanvas->getEdges());

    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    double totalWeight = 0.0;
    const QVector<QPair<qsizetype, qsizetype>> treeEdges =
        algorithms->spanningTree(*graphData, minimum, &totalWeight);

    if (graphData->vertexCount() > 1 && treeEdges.size() != graphData->vertexCount() - 1) {
        ui->logOutput->appendPlainText("Ошибка: граф несвязный, остовное дерево построить нельзя.");
        return;
    }

    graphCanvas->setData(vertices, treeEdges);

    QStringList edges;
    for (const auto &edge : treeEdges) {
        edges << QString("%1-%2").arg(edge.first + 1).arg(edge.second + 1);
    }

    ui->logOutput->appendPlainText(
        QString("%1 остовное дерево: %2\nСуммарная длина: %3")
            .arg(minimum ? "Минимальное" : "Максимальное")
            .arg(edges.isEmpty() ? "нет рёбер" : edges.join(", "))
            .arg(QString::number(totalWeight, 'f', 2)));
}

void MainWindow::on_greedyColoringButton_clicked()
{
    if (animator->isRunning()) {
        animator->stop();
    }

    graphData->setData(graphCanvas->getVertices(), graphCanvas->getEdges());

    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    graphCanvas->setShowEdgeWeights(true);
    applyColoring("Жадная раскраска", algorithms->greedyColoring(*graphData));
}

void MainWindow::on_backtrackingColoringButton_clicked()
{
    if (animator->isRunning()) {
        animator->stop();
    }

    graphData->setData(graphCanvas->getVertices(), graphCanvas->getEdges());

    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    graphCanvas->setShowEdgeWeights(false);
    applyColoring("Раскраска с возвратом", algorithms->backtrackingColoring(*graphData));
}

void MainWindow::on_clearColoringButton_clicked()
{
    graphCanvas->clearVertexColors();
    graphCanvas->setShowEdgeWeights(false);
    ui->logOutput->appendPlainText("Раскраска очищена.");
}

void MainWindow::applyColoring(const QString &title, const QVector<int> &colorIndexes)
{
    QVector<QColor> vertexColors;
    vertexColors.reserve(colorIndexes.size());

    const QVector<QColor> palette = {
        QColor(244, 111, 96),
        QColor(87, 167, 115),
        QColor(89, 143, 206),
        QColor(238, 184, 80),
        QColor(169, 121, 204),
        QColor(77, 181, 183),
        QColor(218, 132, 65),
        QColor(132, 158, 70)
    };

    int colorCount = 0;
    for (int colorIndex : colorIndexes) {
        colorCount = qMax(colorCount, colorIndex + 1);
        if (colorIndex >= 0 && colorIndex < palette.size()) {
            vertexColors.append(palette[colorIndex]);
        } else {
            vertexColors.append(QColor::fromHsv((colorIndex * 47) % 360, 135, 225));
        }
    }

    graphCanvas->clearHighlights();
    graphCanvas->setVertexColors(vertexColors);

    QStringList assignments;
    for (int vertex = 0; vertex < colorIndexes.size(); ++vertex) {
        assignments << QString("%1:%2").arg(vertex + 1).arg(colorIndexes[vertex] + 1);
    }

    ui->logOutput->appendPlainText(
        QString("%1: использовано цветов: %2\nВершины: %3")
            .arg(title)
            .arg(colorCount)
            .arg(assignments.join(", ")));
}

void MainWindow::on_actionNewGraph_triggered()
{
    // Спрашиваем подтверждение, если граф не пустой
    if (graphCanvas->getVertices().size() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Новый граф",
            "Текущий граф будет очищен. Продолжить?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    graphCanvas->clear();
    m_lastSavePath.clear();
    ui->logOutput->appendPlainText("Создан новый граф");
}

void MainWindow::on_actionSaveGraph_triggered()
{
    const QVector<QPointF> &vertices = graphCanvas->getVertices();
    const QVector<QPair<qsizetype, qsizetype>> &edges = graphCanvas->getEdges();

    if (vertices.isEmpty()) {
        QMessageBox::information(this, "Информация", "Граф пуст. Нечего сохранять.");
        return;
    }

    // Выбираем файл для сохранения
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Сохранить граф",
        m_lastSavePath.isEmpty() ? QDir::homePath() + "/graph.json" : m_lastSavePath,
        "Graph Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) {
        return; // Пользователь отменил
    }

    // Сохраняем путь для следующего раза
    m_lastSavePath = fileName;

    // Сохраняем через менеджер
    QString errorMsg;
    if (GraphFileManager::saveToFile(fileName, vertices, edges, graphCanvas->isDirected(), &errorMsg)) {
        ui->logOutput->appendPlainText(
            QString("Граф сохранён: %1 вершин, %2 рёбер\nФайл: %3")
                .arg(vertices.size())
                .arg(edges.size())
                .arg(fileName));
    } else {
        QMessageBox::warning(this, "Ошибка сохранения", errorMsg);
    }
}

void MainWindow::on_actionLoadGraph_triggered()
{
    // Спрашиваем подтверждение, если граф не пустой
    if (graphCanvas->getVertices().size() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Загрузка графа",
            "Текущий граф будет заменён. Продолжить?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    // Выбираем файл для загрузки
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Загрузить граф",
        m_lastSavePath.isEmpty() ? QDir::homePath() : m_lastSavePath,
        "Graph Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) {
        return; // Пользователь отменил
    }

    // Загружаем данные
    QVector<QPointF> vertices;
    QVector<QPair<qsizetype, qsizetype>> edges;
    bool directed = false;

    QString errorMsg;
    if (GraphFileManager::loadFromFile(fileName, vertices, edges, &directed, &errorMsg)) {
        ui->logOutput->appendPlainText(
            QString("Граф загружен: %1 вершин, %2 рёбер\nФайл: %3")
                .arg(vertices.size())
                .arg(edges.size())
                .arg(fileName));

        graphCanvas->setData(vertices, edges);
        graphCanvas->setDirected(directed);
        graphData->setDirected(directed);
        ui->directedToggleButton->setChecked(directed);
        m_lastSavePath = fileName;
        ui->logOutput->appendPlainText("Граф успешно загружен на холст");
    } else {
        QMessageBox::warning(this, "Ошибка загрузки", errorMsg);
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

// --- Меню Вид ---
void MainWindow::on_actionClearLog_triggered()
{
    ui->logOutput->clear();
}

// --- Меню Граф ---
void MainWindow::on_actionAdjacencyMatrix_triggered()
{
    ui->logOutput->appendPlainText("Матрица смежности пока не реализована");
}

void MainWindow::on_actionAdjacencyList_triggered()
{
    ui->logOutput->appendPlainText("Список смежности пока не реализован");
}

void MainWindow::on_directedToggleButton_toggled(bool checked)
{
    graphCanvas->setDirected(checked);
    graphData->setDirected(checked);
    ui->directedToggleButton->setText(checked ? "Ориентированный" : "Неориентированный");
    ui->logOutput->appendPlainText(checked ? "Режим: ориентированный граф"
                                           : "Режим: неориентированный граф");
}

// --- Меню Справка ---
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "О программе",
                       "qt-graph-discrete-math v1.0\n\n"
                       "Программа для построения графов\n"
                       "и визуализации работы алгоритмов\n\n"
                       "Шнякина Елена Александровна - руководитель\n"
                       "Муллагалиев Арслан Артурович - разработчик\n"
                       "Богомолов Богдан - разработчик\n"
                       );
}
