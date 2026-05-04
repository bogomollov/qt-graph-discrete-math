#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "graphcanvas.h"
#include "graphdata.h"
#include "graphalgorithms.h"

// --- Конструктор ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphCanvas(new GraphCanvas(this))
    , graphData(new GraphData)
    , algorithms(new GraphAlgorithms)
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
    // Обновляем данные графа перед запуском алгоритма
    graphData->setData(graphCanvas->getVertices(),
                       graphCanvas->getEdges());

    // Проверка на пустой граф
    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    // Получаем стартовую вершину
    qsizetype startVertex = ui->startVertexSpin->value() - 1;

    QStringList bfsLog = algorithms->bfs(*graphData, startVertex);
    // Вывод лога в текстовое поле
    for (const QString &msg : bfsLog) {
        ui->logOutput->appendPlainText(msg);
    }
}

// --- Обработчик кнопки DFS ---
void MainWindow::on_dfsButton_clicked()
{
    graphData->setData(graphCanvas->getVertices(),
                       graphCanvas->getEdges());

    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    qsizetype startVertex = ui->startVertexSpin->value() - 1;

    QStringList dfsLog = algorithms->dfs(*graphData, startVertex);
    for (const QString &msg : dfsLog) {
        ui->logOutput->appendPlainText(msg);
    }
}

void MainWindow::on_actionSaveGraph_triggered()
{
    ui->logOutput->appendPlainText("Сохранение пока не реализовано");
}

void MainWindow::on_actionLoadGraph_triggered()
{
    ui->logOutput->appendPlainText("Загрузка пока не реализована");
}