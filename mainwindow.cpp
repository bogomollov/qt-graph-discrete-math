#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "graphcanvas.h"

#include <QVBoxLayout>
#include <QPlainTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphCanvas(new GraphCanvas(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("qt-graph-discrete-math"));

    // Холст в ScrollArea
    graphCanvas->setMinimumSize(2000, 2000);
    ui->scrollArea->setWidget(graphCanvas);

    // Лог только для чтения
    ui->logOutput->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- Заглушки ---

void MainWindow::on_bfsButton_clicked()
{
    ui->logOutput->appendPlainText("BFS пока не реализован");
}

void MainWindow::on_dfsButton_clicked()
{
    ui->logOutput->appendPlainText("DFS пока не реализован");
}

void MainWindow::on_actionSaveGraph_triggered()
{
    ui->logOutput->appendPlainText("Сохранение пока не реализовано");
}

void MainWindow::on_actionLoadGraph_triggered()
{
    ui->logOutput->appendPlainText("Загрузка пока не реализована");
}