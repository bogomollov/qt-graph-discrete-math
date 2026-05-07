#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qplaintextedit.h>
#include <QVector>
#include <QString>

class GraphCanvas;
class GraphData;
class GraphAlgorithms;
class GraphAnimator;

class GraphCanvas;

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
    void on_minSpanningTreeButton_clicked();
    void on_maxSpanningTreeButton_clicked();
    void on_greedyColoringButton_clicked();
    void on_backtrackingColoringButton_clicked();
    void on_clearColoringButton_clicked();
    void on_actionNewGraph_triggered();
    void on_actionSaveGraph_triggered();
    void on_actionLoadGraph_triggered();
    void on_actionExit_triggered();
    void on_actionClearLog_triggered();
    void on_actionAdjacencyMatrix_triggered();
    void on_actionAdjacencyList_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    GraphCanvas *graphCanvas;
    GraphData *graphData;
    GraphAlgorithms *algorithms;
    GraphAnimator *animator;
    void buildSpanningTree(bool minimum);
    void applyColoring(const QString &title, const QVector<int> &colorIndexes);

    QString m_lastSavePath;  // Запоминаем последний путь сохранения
};
#endif // MAINWINDOW_H
