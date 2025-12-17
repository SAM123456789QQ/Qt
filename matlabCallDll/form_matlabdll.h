#ifndef FORM_MATLABDLL_H
#define FORM_MATLABDLL_H

#include <QWidget>
#include "global.h"
#include "mythread_e.h"

namespace Ui {
class Form_MatlabDll;
}

class Form_MatlabDll : public QWidget
{
    Q_OBJECT

public:
    explicit Form_MatlabDll(QWidget *parent = nullptr);
    ~Form_MatlabDll();
    void Initialize();
    void upDateDllStade();
    QVector<double> strToVector(const QString& str);

signals:
    void thread_i();
    void thread_c(const QString& str_param,
                  double real_param,
                  int int_param,
                  bool bool_param,
                  const QVector<double>& vec_param,
                  const QVector<QVector<double>>& mat_param,
                  const QVector<QVector<QVector<double>>>& d3_param,
                  const QMap<QString, QVariant>& struct_param,
                  const QList<QVariant>& cell_param);
    void thread_r(const QString &matPath);

private slots:
    void on_tBtn_DllIn_clicked();

    void on_toolButton_clicked();

    void on_btn_readMat_clicked();

    void on_btn_complex_clicked();

private:
    Ui::Form_MatlabDll *ui;
    global *para;
    mythread_e *thd_e;
    QThread *thread,*timeThread;

};

#endif // FORM_MATLABDLL_H
