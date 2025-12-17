#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    para = &global::getInstance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::Initialize
 * @caption 系统初始化
 */
void MainWindow::Initialize()
{
    //  主窗口设置布局和信息
    ui->splitter->setSizes(QList<int>{130,400});
    this->setWindowTitle("淮淮子工具箱");

    //  更新系统时间,同时检测各种库载入状态
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::timeUpate);
    timer->start(1000);

    //  窗口切换 tree 设置右键菜单
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested,this, &MainWindow::onCustomContextMenu);

    this->show();
}

void MainWindow::timeUpate()
{
    ui->lab_time->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh时dd分ss秒"));
    ui->widget_matLabDll->upDateDllStade(); //  更新matlab状态


}

void MainWindow::onCustomContextMenu(const QPoint& point)
{
    if (!ui->treeWidget->itemAt(point))
        return;

    // 创建菜单 + 绑定动作 + 执行
    QMenu menu;
    QAction* actExpand = menu.addAction("全部展开");
    QAction* actCollapse = menu.addAction("全部收起");
    QAction* selected = menu.exec(ui->treeWidget->mapToGlobal(point));

    // 执行对应操作
    if (selected == actExpand) {
        ui->treeWidget->expandAll();
    } else if (selected == actCollapse) {
        ui->treeWidget->collapseAll();
    }

}


//    // 构造输入参数
//    QString file_path = "D:/test.txt";  // 确保文件存在
//    double real_num = 123.45;
//    QVector<QPointF> data;
//    data << QPointF(120.1,21) << QPointF(121.1,22) <<  QPointF(122.1,23);

//    thd_e->callMyFunction(file_path,real_num,data);


void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item)
{
    ui->lab_info->setText(item->text(0));


    if(item->text(0) == "调用matlab库"){
        ui->stackedWidget->setCurrentWidget(ui->page_info);
        ui->textBrowser_info->setText("  将.m代码函数使用matlab的""Library information""用C++模式打包为.dll动态来链接库的格式");
    }
    if(item->text(0) == "调用matlab库示例"){
        ui->stackedWidget->setCurrentWidget(ui->page_matlab);
    }


}

void MainWindow::on_tBtn_DllIn_clicked()
{

}
