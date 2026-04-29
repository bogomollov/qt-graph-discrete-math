#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "graphcanvas.h"

#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphCanvas(new GraphCanvas(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("qt-graph-discrete-math"));

    auto *layout = new QVBoxLayout(ui->centralwidget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(graphCanvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}
