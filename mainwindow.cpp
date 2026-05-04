#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "graphcanvas.h"
#include "graphdata.h"
#include "graphalgorithms.h"
#include "graphanimator.h"
#include <QMessageBox>

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
        ui->logOutput->appendPlainText(
            QString("Граф обновлён: %1 вершин, %2 рёбер")
                .arg(graphData->vertexCount())
                .arg(graphCanvas->getEdges().size()));
    });

    animator->setCanvas(graphCanvas);
    connect(animator, &GraphAnimator::logMessage, ui->logOutput, &QPlainTextEdit::appendPlainText);
    connect(animator, &GraphAnimator::finished, [this]() {
        ui->bfsButton->setText("BFS");
        ui->dfsButton->setText("DFS");
        ui->bfsButton->setEnabled(true);
        ui->dfsButton->setEnabled(true);
    });
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
void MainWindow::on_actionNewGraph_triggered()
{
    ui->logOutput->appendPlainText("Новый граф пока не реализован");
}

void MainWindow::on_actionSaveGraph_triggered()
{
    ui->logOutput->appendPlainText("Сохранение пока не реализовано");
}

void MainWindow::on_actionLoadGraph_triggered()
{
    ui->logOutput->appendPlainText("Загрузка пока не реализована");
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

// --- Меню Справка ---
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "О программе",
                       "qt-graph-discrete-math v0.1\n\n"
                       "Программа для построения графов\n"
                       "и визуализации алгоритмов BFS и DFS.");
}