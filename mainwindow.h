#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QTreeWidgetItem>
#include "global.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Initialize();

private slots:
    void timeUpate();
    void onCustomContextMenu(const QPoint& point);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item);

    void on_tBtn_DllIn_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    global *para;
};
#endif // MAINWINDOW_H
