#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);   //  使用 高DPI 图标和图片

    QApplication a(argc, argv);
    MainWindow w;
    w.Initialize();
    return a.exec();
}
