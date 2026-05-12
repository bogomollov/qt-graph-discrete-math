#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "graphcanvas.h"
#include "graphdata.h"
#include "graphalgorithms.h"
#include "graphanimator.h"
#include "graphfilemanager.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QColor>

class ReadOnlyGraphCanvas : public GraphCanvas
{
public:
    explicit ReadOnlyGraphCanvas(QWidget *parent = nullptr) : GraphCanvas(parent)
    {
        setCursor(Qt::ArrowCursor);
    }
protected:
    void mousePressEvent(QMouseEvent *) override {}
    void mouseMoveEvent(QMouseEvent *) override {}
    void mouseReleaseEvent(QMouseEvent *) override {}
    void keyPressEvent(QKeyEvent *) override {}
};

// --- Конструктор ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphCanvas(new GraphCanvas(this))
    , graphData(new GraphData)
    , algorithms(new GraphAlgorithms)
    , animator(new GraphAnimator(this))
    , spanningTreeWindow(new QWidget(nullptr))
    , spanningTreeCanvas(new ReadOnlyGraphCanvas(spanningTreeWindow))
    , adjacencyMatrixWindow(new QWidget(nullptr))
    , adjacencyListWindow(new QWidget(nullptr))
{
    ui->setupUi(this);

    spanningTreeWindow->setWindowTitle(tr("Остовное дерево"));
    spanningTreeWindow->resize(600, 540);
    spanningTreeWindow->setStyleSheet("background-color: white;");

    QPushButton *transferButton = new QPushButton(tr("Перенести в основное окно"), spanningTreeWindow);
    connect(transferButton, &QPushButton::clicked, [this]() {
        graphCanvas->setData(spanningTreeCanvas->getVertices(), spanningTreeCanvas->getEdges());
        graphCanvas->setShowEdgeWeights(true);
        spanningTreeWindow->hide();
    });

    QVBoxLayout *layout = new QVBoxLayout(spanningTreeWindow);
    layout->setContentsMargins(0, 0, 0, 4);
    layout->addWidget(spanningTreeCanvas);
    layout->addWidget(transferButton);

    spanningTreeCanvas->setShowEdgeWeights(true);
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
        bool ok = false;
        int speedValue = text.toInt(&ok);
        if (ok)
            animator->setAnimationSpeed(1200 - speedValue);
    });

    // Устанавливаем начальную скорость
    bool ok = false;
    int initialSpeedValue = ui->animationSpeedCombo->currentText().toInt(&ok);
    if (ok)
        animator->setAnimationSpeed(1200 - initialSpeedValue);
}

// --- Деструктор ---
MainWindow::~MainWindow()
{
    delete spanningTreeWindow;
    delete adjacencyMatrixWindow;
    delete adjacencyListWindow;
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

void MainWindow::on_dijkstraButton_clicked()
{
    if (animator->isRunning())
        animator->stop();

    graphData->setData(graphCanvas->getVertices(), graphCanvas->getEdges());

    if (graphData->vertexCount() == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    const int from = ui->dijkstraStartSpin->value() - 1;
    const int to   = ui->dijkstraEndSpin->value() - 1;

    if (from >= graphData->vertexCount() || to >= graphData->vertexCount()) {
        ui->logOutput->appendPlainText("Ошибка: вершина не существует.");
        return;
    }

    double totalWeight = 0.0;
    const QVector<QPair<qsizetype, qsizetype>> path =
        algorithms->dijkstra(*graphData, from, to, &totalWeight);

    graphCanvas->clearHighlights();
    graphCanvas->setShowEdgeWeights(false);

    if (path.isEmpty()) {
        ui->logOutput->appendPlainText(
            QString("Дейкстра: путь из %1 в %2 не найден.").arg(from + 1).arg(to + 1));
        return;
    }

    const QColor pathColor(70, 130, 200);
    QStringList steps;
    steps << QString::number(from + 1);
    for (const auto &edge : path) {
        graphCanvas->highlightEdge(static_cast<int>(edge.first),
                                   static_cast<int>(edge.second),
                                   pathColor);
        steps << QString::number(edge.second + 1);
    }
    graphCanvas->setShowEdgeWeights(true);

    ui->logOutput->appendPlainText(
        QString("Дейкстра %1→%2: %3\nДлина пути: %4")
            .arg(from + 1).arg(to + 1)
            .arg(steps.join(" → "))
            .arg(QString::number(totalWeight, 'f', 2)));
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

    spanningTreeCanvas->setData(vertices, treeEdges);
    spanningTreeCanvas->setShowEdgeWeights(true);

    if (!spanningTreeWindow->isVisible()) {
        const QRect mainGeometry = frameGeometry();
        spanningTreeWindow->move(mainGeometry.center() - spanningTreeWindow->rect().center());
        spanningTreeWindow->show();
    }
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

    graphCanvas->setShowEdgeWeights(true);
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
    graphCanvas->setShowEdgeWeights(false);
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
        graphCanvas->setShowEdgeWeights(false);
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
    graphData->setData(graphCanvas->getVertices(), graphCanvas->getEdges());
    const int n = graphData->vertexCount();

    if (n == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    // Build weight matrix
    const QVector<QPointF> &verts = graphData->vertices();
    QVector<QVector<double>> matrix(n, QVector<double>(n, 0.0));
    for (const auto &edge : graphData->edges()) {
        const int a = static_cast<int>(edge.first);
        const int b = static_cast<int>(edge.second);
        const QPointF delta = verts.at(a) - verts.at(b);
        const double w = edgeDisplayWeight(std::hypot(delta.x(), delta.y()));
        matrix[a][b] = w;
        if (!graphData->isDirected())
            matrix[b][a] = w;
    }

    // Rebuild table in the window
    QTableWidget *table = adjacencyMatrixWindow->findChild<QTableWidget *>();
    if (!table) {
        QVBoxLayout *layout = new QVBoxLayout(adjacencyMatrixWindow);
        layout->setContentsMargins(4, 4, 4, 4);
        table = new QTableWidget(adjacencyMatrixWindow);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        layout->addWidget(table);
        adjacencyMatrixWindow->setWindowTitle(tr("Матрица смежности"));
    }

    table->setRowCount(n);
    table->setColumnCount(n);
    QStringList headers;
    for (int i = 0; i < n; ++i)
        headers << QString::number(i + 1);
    table->setHorizontalHeaderLabels(headers);
    table->setVerticalHeaderLabels(headers);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            const double w = matrix[i][j];
            const QString text = (w == 0.0) ? "0" : QString::number(static_cast<long long>(w));
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            table->setItem(i, j, item);
        }
    }

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    adjacencyMatrixWindow->resize(qMin(n * 60 + 60, 700), qMin(n * 30 + 60, 600));

    const QRect mainGeometry = frameGeometry();
    adjacencyMatrixWindow->move(mainGeometry.center() - adjacencyMatrixWindow->rect().center());
    adjacencyMatrixWindow->show();
    adjacencyMatrixWindow->raise();
}

void MainWindow::on_actionAdjacencyList_triggered()
{
    graphData->setData(graphCanvas->getVertices(), graphCanvas->getEdges());
    const int n = graphData->vertexCount();

    if (n == 0) {
        ui->logOutput->appendPlainText("Ошибка: граф пуст!");
        return;
    }

    QTableWidget *table = adjacencyListWindow->findChild<QTableWidget *>();
    if (!table) {
        QVBoxLayout *layout = new QVBoxLayout(adjacencyListWindow);
        layout->setContentsMargins(4, 4, 4, 4);
        table = new QTableWidget(adjacencyListWindow);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setColumnCount(2);
        table->setHorizontalHeaderLabels({tr("Вершина"), tr("Смежные вершины")});
        layout->addWidget(table);
        adjacencyListWindow->setWindowTitle(tr("Список смежности"));
    }

    table->setRowCount(n);
    for (int i = 0; i < n; ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        table->item(i, 0)->setTextAlignment(Qt::AlignCenter);

        QStringList neighbours;
        for (qsizetype nb : graphData->getNeighbors(static_cast<qsizetype>(i)))
            neighbours << QString::number(static_cast<int>(nb) + 1);
        table->setItem(i, 1, new QTableWidgetItem(neighbours.join(", ")));
    }

    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->verticalHeader()->hide();

    adjacencyListWindow->resize(360, qMin(n * 30 + 60, 600));

    const QRect mainGeometry = frameGeometry();
    adjacencyListWindow->move(mainGeometry.center() - adjacencyListWindow->rect().center());
    adjacencyListWindow->show();
    adjacencyListWindow->raise();
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
