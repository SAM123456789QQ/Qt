#include "mythread_e.h"

#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>

mythread_e::mythread_e(QObject *parent) : QObject(parent)
{
    para = &global::getInstance();
}

mythread_e::~mythread_e()
{
    if (myLib_com || myLib_read) {
        myLib_com->unload();
        myLib_read->unload();
        delete myLib_com;
        delete myLib_read;
        myLib_com = nullptr;
        myLib_read = nullptr;
    }
}

void mythread_e::Initialize()
{
    outflag = 0;
    myLib_com = new QLibrary("complex_matlab_func.dll");
    myLib_read = new QLibrary("read_mat_file.dll");

    complexFun = (ComplexFun) myLib_com->resolve("?complex_matlab_func@@YAXHAEAVmwArray@@AEBV1@11111111@Z");
    readMat = (ReadMat) myLib_read->resolve("?read_mat_file@@YAXHAEAVmwArray@@AEBV1@@Z");

    if (!complex_matlab_funcInitialize()){
        outflag= 1;
    }
    if (!read_mat_fileInitialize()){
        outflag= 1;
    }
    if(outflag == 1)
    {
        para->setDlloutFlag(outflag);
        qDebug() << "MATLAB动态链接库初始化失败，请检查系统环境配置！";
        return ;
    }
    outflag = 2;
    para->setDlloutFlag(outflag);
}

void mythread_e::callComplexFun(const QString &str_param, double real_param, int int_param, bool bool_param, const QVector<double> &vec_param, const QVector<QVector<double>> &mat_param, const QVector<QVector<QVector<double>>> &d3_param, const QMap<QString, QVariant> &struct_param, const QList<QVariant> &cell_param)
{

    if(outflag != 2 || mat_param.isEmpty() || d3_param.isEmpty() || struct_param.isEmpty() || cell_param.isEmpty()) return;

    mwArray mwA_str(str_param.toUtf8().constData());

    mwArray mwA_real(1, 1, mxDOUBLE_CLASS, mxREAL);
    mwA_real(1, 1) = real_param;

    mwArray mwA_int(1, 1, mxINT32_CLASS, mxREAL);
    mwA_int(1, 1) = int_param;

    mwArray mwA_bool(1, 1, mxLOGICAL_CLASS, mxREAL);
    mwA_bool(1, 1) = bool_param;

    mwArray mwA_vec(1, vec_param.size(), mxDOUBLE_CLASS, mxREAL);
    std::vector<double> tempVec(vec_param.begin(), vec_param.end());    // MATLAB是列优先，对于行向量直接复制
    mwA_vec.SetData(tempVec.data(), vec_param.size());

    mwArray mwA_mat(mat_param.size(), mat_param.first().size(), mxDOUBLE_CLASS, mxREAL);
    std::vector<double> flatData(mat_param.size() * mat_param.first().size());
    for (int i = 0; i < mat_param.size(); ++i) {
        if (mat_param[i].size() != mat_param.first().size()) {
            throw std::runtime_error("矩阵各行长度不一致");
        }
        for (int j = 0; j < mat_param.first().size(); ++j) {
            flatData[j * mat_param.size() + i] = mat_param[i][j];  // 列优先存储
        }
    }
    mwA_mat.SetData(flatData.data(), mat_param.size() * mat_param.first().size());

    int dim1 = d3_param.size();         // 一维
    int dim2 = d3_param[0].size();      // 二维
    int dim3 = d3_param[0][0].size();   // 三维
    // 验证维度一致性
    for (int i = 0; i < dim1; ++i) {
        if (d3_param[i].size() != dim2) {
            throw std::runtime_error("三维数组第二维不一致");
        }
        for (int j = 0; j < dim2; ++j) {
            if (d3_param[i][j].size() != dim3) {
                throw std::runtime_error("三维数组第三维不一致");
            }
        }
    }
    // 展平为MATLAB的列优先顺序
    flatData = std::vector<double>(dim1 * dim2 * dim3);
    int index = 0;
    for (int k = 0; k < dim3; ++k) {          // 第三维
        for (int j = 0; j < dim2; ++j) {      // 第二维
            for (int i = 0; i < dim1; ++i) {  // 第一维
                flatData[index++] = d3_param[i][j][k];
            }
        }
    }
    mwSize dims[3] = {(mwSize)dim1, (mwSize)dim2, (mwSize)dim3};
    mwArray mwA_d3(3, dims, mxDOUBLE_CLASS, mxREAL);
    mwA_d3.SetData(flatData.data(), dim1 * dim2 * dim3);

    QVector<const char*> fieldNames;
    for (auto it = struct_param.constBegin(); it != struct_param.constEnd(); ++it) {
        fieldNames.append(it.key().toLatin1().constData());
    }

    qDebug() << __FILE__ << __LINE__;

    mwArray mwA_struct = createStructParam(struct_param);

    qDebug() << __FILE__ << __LINE__;

    mwArray mwA_cell(1, cell_param.size(), mxCELL_CLASS);

    for (int i = 0; i < cell_param.size(); ++i) {
        const QVariant& cell = cell_param[i];

        if (cell.type() == QVariant::Double) {
            mwArray arr(1, 1, mxDOUBLE_CLASS, mxREAL);
            arr(1, 1) = cell.toDouble();
            mwA_cell(1, i + 1) = arr;
        } else if (cell.type() == QVariant::Int) {
            mwArray arr(1, 1, mxINT32_CLASS, mxREAL);
            arr(1, 1) = cell.toInt();
            mwA_cell(1, i + 1) = arr;
        } else if (cell.type() == QVariant::String) {
            mwA_cell(1, i + 1) = mwArray(cell.toString().toUtf8().constData());
        } else if (cell.type() == QVariant::Bool) {
            mwArray arr(1, 1, mxLOGICAL_CLASS, mxREAL);
            arr(1, 1) = cell.toBool();
            mwA_cell(1, i + 1) = arr;
        } else if (cell.type() == QVariant::List) {
            // 处理向量
            QList<QVariant> list = cell.toList();
            if(list.isEmpty()) return;
            QVector<double> vec;
            for (const QVariant& item : list) {
                if (item.canConvert<double>()) {
                    vec.append(item.toDouble());
                }
            }
            mwArray arr(1, vec.size(), mxDOUBLE_CLASS, mxREAL);
            std::vector<double> tempVec(vec.begin(), vec.end());
            arr.SetData(tempVec.data(), vec.size());
            mwA_cell(1, i + 1) = arr;
        } else if (cell.type() == QVariant::Map) {
            // 处理嵌套结构体
            QMap<QString, QVariant> map = cell.toMap();
            mwA_cell(1, i + 1) = createStructParam(map);
        }
    }

        qDebug() << __FILE__ << __LINE__;

    // 准备输出参数
    mwArray resultStruct;

    // 调用MATLAB函数
    qDebug() << "开始调用MATLAB函数...";
    qDebug() << "  字符串参数长度:" << str_param.length();
    qDebug() << "  实数参数:" << real_param;
    qDebug() << "  整数参数:" << int_param;
    qDebug() << "  布尔参数:" << bool_param;
    qDebug() << "  向量长度:" << vec_param.size();
    qDebug() << "  矩阵大小:" << mat_param.size() << "x"
             << (mat_param.empty() ? 0 : mat_param[0].size());
    qDebug() << "  三维数组大小:" << d3_param.size() << "x"
             << (d3_param.empty() ? 0 : d3_param[0].size()) << "x"
             << (d3_param.empty() || d3_param[0].empty() ? 0 : d3_param[0][0].size());
    qDebug() << "  结构体字段数:" << struct_param.size();
    qDebug() << "  元胞数组大小:" << cell_param.size();


    try {
        complexFun(1, resultStruct,mwA_str, mwA_real, mwA_int, mwA_bool,mwA_vec, mwA_mat, mwA_d3,mwA_struct, mwA_cell);
    } catch (const mwException& e) {
        qCritical() << "MATLAB异常:" << e.what();
    } catch (const std::exception& e) {
        qCritical() << "C++异常:" << e.what();
    }

    qDebug() << "MATLAB函数调用成功";

}


void mythread_e::callReadMat(const QString &matPath)
{

    // 1. 初始化检查
    if (outflag != 2) return;

    // 3. 定义输入/输出参数（MATLAB 函数参数对应）
    mwArray mwA(matPath.toUtf8().constData());   // 输入：mat文件路径
    mwArray outputStruct;                        // 输出：MATLAB 结构体

    // 4. 调用 MATLAB 打包的函数（函数名格式：库名+函数名）
    // 函数参数：输出参数数、输出参数数组、输入参数数、输入参数数组
    readMat(1, outputStruct, mwA);

    // 5. 解析输出结构体（核心：提取字段值）
//    if (mxIsStruct(outputStruct))
//    {
//        // 5.1 提取元数据字段
//        mxArray* metaFilePath = mxGetField(outputStruct, 0, "meta_file_path");
//        mxArray* varCount = mxGetField(outputStruct, 0, "meta_variable_count");
//        mxArray* varNames = mxGetField(outputStruct, 0, "meta_variable_names");

//        // 转换为 Qt 可识别的类型
//        QString metaPath = mxArrayToString(metaFilePath);
//        int variableCount = mxGetScalar(varCount);  // 数值类型直接取标量

//        qDebug() << "MAT文件路径：" << metaPath;
//        qDebug() << "变量数量：" << variableCount;

//        // 5.2 解析变量名列表（cell 数组）
//        if (mxIsCell(varNames)) {
//            int cellCount = mxGetNumberOfElements(varNames);
//            qDebug() << "变量名列表：";
//            for (int i = 0; i < cellCount; i++) {
//                mxArray* cellElement = mxGetCell(varNames, i);
//                QString varName = mxArrayToString(cellElement);
//                qDebug() << " - " << varName;

//                // 5.3 提取每个变量的值（根据变量类型解析）
//                mxArray* varValue = mxGetField(outputStruct, 0, varName.toLocal8Bit().constData());
//                if (varValue != nullptr) {
//                    // 示例1：解析数值标量（如 double）
//                    if (mxIsDouble(varValue) && mxGetNumberOfElements(varValue) == 1) {
//                        double value = mxGetScalar(varValue);
//                        qDebug() << "  " << varName << " = " << value;
//                    }
//                    // 示例2：解析字符串
//                    else if (mxIsChar(varValue)) {
//                        QString strValue = mxArrayToString(varValue);
//                        qDebug() << "  " << varName << " = " << strValue;
//                    }
//                    // 示例3：解析矩阵（double 数组）
//                    else if (mxIsDouble(varValue)) {
//                        int rows = mxGetM(varValue);
//                        int cols = mxGetN(varValue);
//                        double* data = mxGetPr(varValue);  // 获取矩阵数据指针
//                        qDebug() << "  " << varName << " 矩阵维度：" << rows << "x" << cols;
//                        // 打印矩阵前几个元素（示例）
//                        for (int j = 0; j < qMin(5, rows*cols); j++) {
//                            qDebug() << "    元素[" << j << "] = " << data[j];
//                        }
//                    }
//                }
//            }
//        }
//    }

}

mwArray mythread_e::createStructParam(const QMap<QString, QVariant> &structData)
{
    int numFields = structData.size();
    if (numFields == 0) {
        return mwArray();
    }

    // 验证并清理字段名
    QVector<const char*> fieldNames;
    QVector<QByteArray> fieldNameBytes; // 保持生命周期

    for (auto it = structData.constBegin(); it != structData.constEnd(); ++it) {
        QString fieldName = it.key();

        // 1. 检查字段名是否为空
        if (fieldName.isEmpty()) {
            qWarning() << "结构体字段名为空，跳过";
            continue;
        }

        // 2. 清理字段名（移除MATLAB不允许的字符）
        // MATLAB字段名规则：以字母开头，只包含字母、数字、下划线
        QString cleanedName = fieldName;
        cleanedName = cleanedName.replace(QRegularExpression("[^a-zA-Z0-9_]"), "_");

        // 确保以字母开头
        if (!cleanedName[0].isLetter()) {
            cleanedName = "field_" + cleanedName;
        }

        // 3. 检查字段名长度
        if (cleanedName.length() > 63) { // MATLAB字段名最大63个字符
            cleanedName = cleanedName.left(63);
        }

        qDebug() << "原始字段名:" << fieldName << "-> 清理后:" << cleanedName;

        QByteArray nameBytes = cleanedName.toLatin1();
        fieldNameBytes.append(nameBytes);
        fieldNames.append(fieldNameBytes.last().constData());
    }

    if (fieldNames.isEmpty()) {
        return mwArray();
    }

    // 创建结构体
    mwArray mwA_struct(1, 1, fieldNames.size(), fieldNames.data());

    // 填充字段值
    int fieldIndex = 0;
    for (auto it = structData.constBegin(); it != structData.constEnd(); ++it) {
        if (fieldIndex >= fieldNames.size()) break;

        QString originalFieldName = it.key();
        QByteArray fieldNameByte = fieldNameBytes[fieldIndex];
        const char* fieldName = fieldNameByte.constData();
        QVariant value = it.value();

        try {
            if (value.type() == QVariant::Double) {
                mwArray fieldValue(1, 1, mxDOUBLE_CLASS, mxREAL);
                fieldValue(1,1) = value.toDouble();
                mwA_struct(fieldName, 1, 1) = fieldValue;
            } else if (value.type() == QVariant::Int) {
                mwArray fieldValue(1, 1, mxINT32_CLASS, mxREAL);
                fieldValue(1,1) = value.toInt();
                mwA_struct(fieldName, 1, 1) = fieldValue;
            } else if (value.type() == QVariant::String) {
                mwArray fieldValue(value.toString().toUtf8().constData());
                mwA_struct(fieldName, 1, 1) = fieldValue;
            } else if (value.type() == QVariant::List) {
                QList<QVariant> list = value.toList();
                QVector<double> vec;
                for (const QVariant& item : list) {
                    if (item.canConvert<double>()) {
                        vec.append(item.toDouble());
                    }
                }
                if (!vec.isEmpty()) {
                    mwArray fieldValue(1, vec.size(), mxDOUBLE_CLASS, mxREAL);
                    std::vector<double> tempVec(vec.begin(), vec.end());
                    fieldValue.SetData(tempVec.data(), vec.size());
                    mwA_struct(fieldName, 1, 1) = fieldValue;
                }
            }
        } catch (const mwException& e) {
            qCritical() << "设置字段" << fieldName << "时出错:" << e.what();
        }

        fieldIndex++;
    }

    return mwA_struct;
}


//mythread_e::ResultData mythread_e::parseResultStruct(const mwArray &resultStruct)
//{

//    ResultData result;

//    if (resultStruct.IsEmpty()) {
//        qWarning() << "返回的结构体为空";
//        return result;
//    }

//    if (!resultStruct.IsStruct()) {
//        qWarning() << "返回值不是结构体";
//        return result;
//    }

//    try {
//        // 1. 字符串输出
//        if (resultStruct.IsField("str_output")) {
//            mwArray strField = resultStruct("str_output", 1, 1);
//            result.str_output = extractString(strField);
//        }

//        // 2. 实数输出
//        if (resultStruct.IsField("real_output")) {
//            mwArray realField = resultStruct("real_output", 1, 1);
//            result.real_output = extractDouble(realField);
//        }

//        // 3. 整数输出
//        if (resultStruct.IsField("int_output")) {
//            mwArray intField = resultStruct("int_output", 1, 1);
//            result.int_output = extractInt(intField);
//        }

//        // 4. 布尔输出
//        if (resultStruct.IsField("bool_output")) {
//            mwArray boolField = resultStruct("bool_output", 1, 1);
//            result.bool_output = extractBool(boolField);
//        }

//        // 5. 向量输出
//        if (resultStruct.IsField("vec_output")) {
//            mwArray vecField = resultStruct("vec_output", 1, 1);
//            result.vec_output = extractVector(vecField);
//        }

//        // 6. 矩阵输出
//        if (resultStruct.IsField("mat_output")) {
//            mwArray matField = resultStruct("mat_output", 1, 1);
//            result.mat_output = extractMatrix(matField);
//        }

//        // 7. 三维数组统计信息
//        if (resultStruct.IsField("d3_stats")) {
//            mwArray d3StatsField = resultStruct("d3_stats", 1, 1);
//            result.d3_stats = extractD3Stats(d3StatsField);
//        }

//        // 8. 三维FFT均值
//        if (resultStruct.IsField("d3_fft_mean")) {
//            mwArray d3FftField = resultStruct("d3_fft_mean", 1, 1);
//            result.d3_fft_mean = extractDouble(d3FftField);
//        }

//        // 9. 三维切片
//        if (resultStruct.IsField("d3_slice")) {
//            mwArray d3SliceField = resultStruct("d3_slice", 1, 1);
//            result.d3_slice = extractMatrix(d3SliceField);
//        }

//        // 10. 大型矩阵统计
//        if (resultStruct.IsField("large_mat_stats")) {
//            mwArray largeStatsField = resultStruct("large_mat_stats", 1, 1);
//            result.large_mat_stats = extractLargeMatStats(largeStatsField);
//        }

//        // 11. FFT均值
//        if (resultStruct.IsField("fft_mean_abs")) {
//            mwArray fftMeanField = resultStruct("fft_mean_abs", 1, 1);
//            result.fft_mean_abs = extractDouble(fftMeanField);
//        }

//        // 12. LU条件数
//        if (resultStruct.IsField("lu_cond")) {
//            mwArray luCondField = resultStruct("lu_cond", 1, 1);
//            result.lu_cond = extractDouble(luCondField);
//        }

//        // 13. 条件计算结果
//        if (resultStruct.IsField("cond_output")) {
//            mwArray condField = resultStruct("cond_output", 1, 1);
//            result.cond_output = extractDouble(condField);
//        }

//        // 14. 结构体输出
//        if (resultStruct.IsField("struct_output")) {
//            mwArray structField = resultStruct("struct_output", 1, 1);
//            result.struct_output = extractStructOutput(structField);
//        }

//        // 15. 元胞数组输出
//        if (resultStruct.IsField("cell_output")) {
//            mwArray cellField = resultStruct("cell_output", 1, 1);
//            result.cell_output = extractCellArray(cellField);
//        }

//        // 16. 计算时间
//        if (resultStruct.IsField("computation_time")) {
//            mwArray timeField = resultStruct("computation_time", 1, 1);
//            result.computation_time = extractDouble(timeField);
//        }

//        qDebug() << "结果解析完成";
//        qDebug() << "  字符串结果:" << result.str_output;
//        qDebug() << "  计算耗时:" << result.computation_time << "秒";

//    } catch (const mwException& e) {
//        qCritical() << "解析结果时MATLAB异常:" << e.what();
//    } catch (const std::exception& e) {
//        qCritical() << "解析结果时C++异常:" << e.what();
//    }

//    return result;
//}

