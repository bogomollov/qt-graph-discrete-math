#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QQuickWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , graphView(new QQuickWidget(this))
{
    ui->setupUi(this);
    setWindowTitle(tr("qt-graph-discrete-math"));

    auto *layout = new QVBoxLayout(ui->centralwidget);
    layout->setContentsMargins(0, 0, 0, 0);

    graphView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    graphView->setSource(QUrl(QStringLiteral("qrc:/qt/qml/GraphEditor.qml")));
    layout->addWidget(graphView);
}

MainWindow::~MainWindow()
{
    delete ui;
}
