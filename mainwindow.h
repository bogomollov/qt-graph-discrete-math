#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qplaintextedit.h>

class GraphCanvas;
class GraphData;
class GraphAlgorithms;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_bfsButton_clicked();
    void on_dfsButton_clicked();
    void on_actionSaveGraph_triggered();
    void on_actionLoadGraph_triggered();

private:
    Ui::MainWindow *ui;
    GraphCanvas *graphCanvas;
    GraphData *graphData;
    GraphAlgorithms *algorithms;
};
#endif // MAINWINDOW_H
