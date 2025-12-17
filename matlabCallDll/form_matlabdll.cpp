#include "form_matlabdll.h"
#include "ui_form_matlabdll.h"
#include <QThread>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

Form_MatlabDll::Form_MatlabDll(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_MatlabDll)
{
    ui->setupUi(this);
    para = &global::getInstance();
    Initialize();
}

Form_MatlabDll::~Form_MatlabDll()
{
    // 停止并释放线程资源
    thread->quit();    // 终止线程事件循环
    thread->wait();    // 等待线程结束
    delete thread;
    if (timeThread) {  // 处理timeThread（若已初始化）
        timeThread->quit();
        timeThread->wait();
        delete timeThread;
    }
    delete ui;
}

void Form_MatlabDll::Initialize()
{
    //  初始化matlab
    thread = new QThread();
    thd_e = new mythread_e();
    thd_e->moveToThread(thread);
    thread->start();
    connect(this,&Form_MatlabDll::thread_i,thd_e,&mythread_e::Initialize);
    connect(this,&Form_MatlabDll::thread_r,thd_e,&mythread_e::callReadMat);
    connect(this,&Form_MatlabDll::thread_c,thd_e,&mythread_e::callComplexFun);

    //  默认为初始路径
    ui->lEdit_matPath->setText(QCoreApplication::applicationDirPath());

    // 一次性注册所有需要的类型
    static bool typesRegistered = false;
    if (!typesRegistered) {
        qRegisterMetaType<QVector<double>>("QVector<double>");
        qRegisterMetaType<QVector<QVector<double>>>("QVector<QVector<double>>");
        qRegisterMetaType<QVector<QVector<QVector<double>>>>("QVector<QVector<QVector<double>>>");
        qRegisterMetaType<QMap<QString, QVariant>>("QMap<QString,QVariant>");
        qRegisterMetaType<QList<QVariant>>("QList<QVariant>");
    }

}

void Form_MatlabDll::upDateDllStade()
{
    int DlloutFlag = para->getDlloutFlag();
    //  关于动态链接库载入状态
    {
        if(DlloutFlag == -1){
            ui->lab_DllinfoIco->setPixmap(QPixmap(":/matLabWidget/resource/matLabWidget/failure.png"));
            ui->lab_Dllinfo->setText("状态：Matlab动态链接库未载入!");
        }
        if(DlloutFlag == 0){
            ui->lab_DllinfoIco->setPixmap(QPixmap(":/matLabWidget/resource/matLabWidget/failure.png"));
            ui->lab_Dllinfo->setText("状态：Matlab动态链接库正在载入!");
        }
        if(DlloutFlag == 1){
            ui->lab_DllinfoIco->setPixmap(QPixmap(":/matLabWidget/resource/matLabWidget/failure.png"));
            ui->lab_Dllinfo->setText("状态：Matlab动态链接库载入失败!");
        }
        if(DlloutFlag == 2){
            ui->lab_DllinfoIco->setPixmap(QPixmap(":/matLabWidget/resource/matLabWidget/Success.png"));
            ui->lab_Dllinfo->setText("状态：Matlab动态链接库载入完成!");
        }
    }
}

QVector<double> Form_MatlabDll::strToVector(const QString &str)
{
    QVector<double> result;
    for (const QString& s : str.split(','))
        result.append(s.trimmed().toDouble());
    return result;
}


void Form_MatlabDll::on_tBtn_DllIn_clicked()
{
    emit thread_i();
}

void Form_MatlabDll::on_toolButton_clicked()
{
    QString matPath = QFileDialog::getOpenFileName(this,tr("请选择需要读取的.mat文件的路径"),ui->lEdit_matPath->text(),tr("MAT文件 (*.mat)"));
    if(!matPath.isEmpty())
        ui->lEdit_matPath->setText(matPath);
}

void Form_MatlabDll::on_btn_readMat_clicked()
{
    if(para->getDlloutFlag() != 2)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先载入动态链接库"));
        return;
    }

    QString matPath = ui->lEdit_matPath->text();
    if (!matPath.endsWith(".mat", Qt::CaseInsensitive)) { // 忽略大小写
        QMessageBox::warning(this, tr("提示"), tr("请选择后缀为.mat的文件！"));
        return;
    }

    qDebug() << matPath;
    emit thread_r(matPath);

}

void Form_MatlabDll::on_btn_complex_clicked()
{
    if(para->getDlloutFlag() != 2)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先载入动态链接库"));
        return;
    }


#if 0
    // 准备测试数据
    QString str_param = "Hello MATLAB from Qt";
    double real_param = 3.14159;
    int int_param = 42;
    bool bool_param = true;

    // 向量参数
    QVector<double> vec_param = {1.0, 2.0, 3.0, 4.0, 5.0};

    // 矩阵参数 (2x3)
    QVector<QVector<double>> mat_param = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0}
    };

    // 三维数组参数 (2x2x2)
    QVector<QVector<QVector<double>>> d3_param(2);
    for (int i = 0; i < 2; ++i) {
        d3_param[i].resize(2);
        for (int j = 0; j < 2; ++j) {
            d3_param[i][j].resize(2);
            for (int k = 0; k < 2; ++k) {
                d3_param[i][j][k] = (i + 1) * 100 + (j + 1) * 10 + (k + 1);
            }
        }
    }

    // 结构体参数
    QMap<QString, QVariant> struct_param;
    struct_param["field1"] = 100.5;
    struct_param["field2"] = QVariantList({1.0, 2.0, 3.0, 4.0});
    struct_param["field3"] = "struct_field";

    // 元胞数组参数
    QList<QVariant> cell_param;
    cell_param.append(123);
    cell_param.append("cell_string");
    cell_param.append(true);
    cell_param.append(QVariantList({1.1, 2.2}));

    QMap<QString, QVariant> nestedStruct;
    nestedStruct["subfield"] = 456;
    cell_param.append(QVariant::fromValue(nestedStruct));

#else
    // 准备测试数据
    QString str_param = ui->ledit_str->text();
    double real_param = ui->dsbox_real->value();
    int int_param = ui->sbox_int->value();
    bool bool_param = ui->cbox_bool->currentText().contains("True");
    // 向量参数
    QVector<double> vec_param = strToVector(ui->ledit_vec->text());
    // 矩阵参数 (2x3)
    QVector<QVector<double>> mat_param= {
        strToVector(ui->ledit_mat1->text()),
        strToVector(ui->ledit_mat2->text())
    };
    // 三维数组参数
    QVector<QVector<QVector<double>>> d3_param(3);
    for(auto str : ui->ledit_3d1->text().split('|')){
        d3_param[0].append(strToVector(str));
    }
    for(auto str : ui->ledit_3d2->text().split('|')){
        d3_param[1].append(strToVector(str));
    }
    for(auto str : ui->ledit_3d3->text().split('|')){
        d3_param[2].append(strToVector(str));
    }

    // 结构体参数
    QMap<QString, QVariant> struct_param;
    QStringList field1 = ui->ledit_struct1->text().split('=');
    struct_param[field1[0].remove(QRegularExpression(R"([\[\]\"])")).trimmed()] = field1[1].trimmed().toDouble();;

    QStringList field2 = ui->ledit_struct2->text().split('=');
    QVariantList list;
    for(double value : strToVector(field2[1].trimmed())){
        list.append(value);
    }
    struct_param[field2[0].remove(QRegularExpression(R"([\[\]\"])")).trimmed()] = list;

    QStringList field3 = ui->ledit_struct3->text().split('=');
    struct_param[field3[0].remove(QRegularExpression(R"([\[\]\"])")).trimmed()] = field3[1].trimmed();

    // 元胞数组参数
    QList<QVariant> cell_param;
    cell_param.append(ui->ledit_cell1->text().toInt());
    cell_param.append(ui->ledit_cell2->text());
    QVariantList listc;
    for(double value : strToVector(ui->ledit_cell3->text())){
        listc.append(value);
    }
    cell_param.append(listc);
    cell_param.append(QVariant::fromValue(struct_param));

#endif

    emit thread_c(str_param,real_param,int_param,bool_param,vec_param,mat_param,d3_param,struct_param,cell_param);
}
