#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("qt-graph-discrete-math");

    // Загрузка стиля Windows XP
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = styleFile.readAll();
        a.setStyleSheet(style);
        styleFile.close();
    }

    MainWindow w;
    w.show();
    // todo ...
    return QCoreApplication::exec();
}
