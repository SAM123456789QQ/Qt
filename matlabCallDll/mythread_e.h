#ifndef MYTHREAD_E_H
#define MYTHREAD_E_H

#include <QObject>
#include <QLibrary>
#include <QString>
#include <QPointF>

#include "mat.h"
#include "matrix.h"
#include "mclmcrrt.h"
#include "mclcppclass.h"
#include "read_mat_file.h"
#include "complex_matlab_func.h"
#include "global.h"

typedef void (MW_CALL_CONV *ComplexFun)(int nargout,
                                        mwArray& result_struct,
                                        const mwArray& str_param,
                                        const mwArray& real_param,
                                        const mwArray& int_param,
                                        const mwArray& bool_param,
                                        const mwArray& vec_param,
                                        const mwArray& mat_param,
                                        const mwArray& d3_param,
                                        const mwArray& struct_param,
                                        const mwArray& cell_param);

typedef void (MW_CALL_CONV *ReadMat)(int nargout, mwArray& result_struct, const mwArray& mat_file_path);

class mythread_e : public QObject
{
    Q_OBJECT
public:
    // 结构体定义，用于Qt端的结果解析
    struct D3Stats {
        double mean_val;
        double median_val;
        double std_val;
        double min_val;
        double max_val;
    };

    struct LargeMatStats {
        double mean_val;
        double median_val;
        double std_val;
        double var_val;
        double min_val;
        double max_val;
    };

    struct StructOutput {
        double field1_plus;
        QVector<double> field2_times;
    };

    struct ResultData {
        // 基本类型结果
        QString str_output;
        double real_output;
        int int_output;
        bool bool_output;

        // 数组类型结果
        QVector<double> vec_output;
        QVector<QVector<double>> mat_output;

        // 三维数组结果
        D3Stats d3_stats;
        double d3_fft_mean;
        QVector<QVector<double>> d3_slice;

        // 大型计算结果
        LargeMatStats large_mat_stats;
        double fft_mean_abs;
        double lu_cond;

        // 条件计算结果
        double cond_output;

        // 结构体和元胞数组结果
        StructOutput struct_output;
        QList<QVariant> cell_output;

        // 计算耗时
        double computation_time;
    };

public:
    explicit mythread_e(QObject *parent = nullptr);
    ~mythread_e();
    void Initialize();
    void callComplexFun(const QString& str_param,
                        double real_param,
                        int int_param,
                        bool bool_param,
                        const QVector<double>& vec_param,
                        const QVector<QVector<double>>& mat_param,
                        const QVector<QVector<QVector<double>>>& d3_param,
                        const QMap<QString, QVariant>& struct_param,
                        const QList<QVariant>& cell_param);

    void callReadMat(const QString &matPath);
    ResultData parseResultStruct(const mwArray& resultStruct);
    mwArray createStructParam(const QMap<QString, QVariant>& structData);   //  参数辅助函数

private:

    int outflag;
    global *para;

    QLibrary *myLib_com;
    QLibrary *myLib_read;
    ComplexFun complexFun;
    ReadMat readMat;
};

#if 0

% 用于读取.mat文件的MATLAB函数
% 输入参数：
%   mat_file_path: 字符串，.mat文件路径
% 返回值：
%   result_struct: 结构体，直接包含.mat文件中的所有变量作为字段
%                  同时包含元数据信息

function result_struct = read_mat_file(mat_file_path)
    % 检查输入参数合法性
    disp(mat_file_path)
    if ~ischar(mat_file_path) && ~isstring(mat_file_path)
        error('输入参数必须是字符串类型的文件路径');
    end
    if isstring(mat_file_path)
        mat_file_path = char(mat_file_path);
    end

    % 检查文件是否存在
    if ~exist(mat_file_path, 'file')
        error('指定的.mat文件不存在: %s', mat_file_path);
    end

    % 检查文件是否为.mat文件
    [~, ~, ext] = fileparts(mat_file_path);
    if ~strcmpi(ext, '.mat')
        error('指定的文件不是.mat文件: %s', mat_file_path);
    end

    % 读取.mat文件中的所有变量
    mat_contents = load(mat_file_path);

    % 获取所有变量名
    var_names = fieldnames(mat_contents);

    % 构建返回结构体，直接包含所有变量作为字段
    % 首先添加元数据信息
    result_struct = struct();
    result_struct.meta_file_path = mat_file_path;
    result_struct.meta_variable_count = length(var_names);
    result_struct.meta_variable_names = var_names;
    result_struct.meta_timestamp = datestr(now());

    % 直接将所有变量作为结构体字段添加
    % 这样Qt可以直接访问result_struct.var_name来获取变量值
    for i = 1:length(var_names)
        var_name = var_names{i};
        var_value = mat_contents.(var_name);

        % 直接将变量添加为结构体字段
        result_struct.(var_name) = var_value;
    end

    % 保存读取结果，便于调试
    % save('read_mat_file_result.mat', 'result_struct');
end

% 用于封装成DLL的复杂MATLAB函数
% 覆盖多种MATLAB参数类型，并包含较大计算量
% 输入参数：
%   str_param: 字符串，示例字符串参数
%   real_param: 实数，示例实数参数
%   int_param: 整数，示例整数参数
%   bool_param: 布尔值，示例布尔参数
%   vec_param: 一维数组，示例向量参数
%   mat_param: 二维数组，示例矩阵参数
%   3d_param: 三维数组，示例三维数组参数
%   struct_param: 结构体，包含多种类型字段
%   cell_param: 元胞数组，包含多种类型元素
% 返回值：
%   result_struct: 结构体，包含多种计算结果

function result_struct = complex_matlab_func(str_param, real_param, int_param, bool_param, vec_param, mat_param, d3_param, struct_param, cell_param)
    % 保存所有输入参数，便于调试和验证
    save('complex_matlab_func_input.mat', ...
         'str_param', 'real_param', 'int_param', 'bool_param', ...
         'vec_param', 'mat_param', 'd3_param', 'struct_param', 'cell_param');

    % ---------------------- 输入参数合法性检查（增强版） ----------------------
    % 字符串检查
    if ~ischar(str_param) && ~isstring(str_param)
        error('第一个参数必须是字符串类型');
    end
    if isstring(str_param)
        str_param = char(str_param);
    end

    % 实数检查 - 允许整数输入
    if ~isnumeric(real_param) || numel(real_param) ~= 1 || ~isreal(real_param)
        error('第二个参数必须是单个实数');
    end
    % 转换为double确保类型一致
    real_param = double(real_param);

    % 整数检查 - 允许浮点数输入，取整
    if ~isnumeric(int_param) || numel(int_param) ~= 1 || ~isreal(int_param)
        error('第三个参数必须是单个数值');
    end
    % 转换为double，MATLAB内部统一使用double
    int_param = double(int_param);

    % 布尔值检查 - 允许数值输入
    if islogical(bool_param) && numel(bool_param) == 1
        % 已经是布尔值，保持原样
    elseif isnumeric(bool_param) && numel(bool_param) == 1
        % 数值转换为逻辑值
        bool_param = logical(bool_param);
    else
        error('第四个参数必须是单个布尔值或数值');
    end

    % 向量检查 - 确保是double类型
    if ~isnumeric(vec_param) || ~isreal(vec_param) || ndims(vec_param) ~= 2 || ...
       (size(vec_param, 1) ~= 1 && size(vec_param, 2) ~= 1)
        error('第五个参数必须是一维实数数组');
    end
    vec_param = double(vec_param);

    % 矩阵检查 - 确保是double类型
    if ~isnumeric(mat_param) || ~isreal(mat_param) || ndims(mat_param) ~= 2
        error('第六个参数必须是二维实数数组');
    end
    mat_param = double(mat_param);

    % 三维数组检查 - 确保是double类型
    if ~isnumeric(d3_param) || ~isreal(d3_param) || ndims(d3_param) ~= 3
        error('第七个参数必须是三维实数数组');
    end
    d3_param = double(d3_param);

    % 结构体检查 - 更宽松的检查
    if ~isstruct(struct_param)
        error('第八个参数必须是结构体');
    end

    % 元胞数组检查
    if ~iscell(cell_param)
        error('第九个参数必须是元胞数组');
    end

    % ---------------------- 核心计算（修复类型兼容性） ----------------------

    % 记录开始时间
    tic;

    % 1. 生成大型随机矩阵（用于后续计算）
    large_mat = randn(1000, 1000); % 1000x1000随机矩阵

    % 2. 矩阵乘法（较大计算量）
    mat_mult_result = large_mat * large_mat; % 1000x1000矩阵乘法

    % 3. 矩阵分解（LU分解，计算量大）
    [L, U, P] = lu(large_mat);

    % 4. FFT变换（计算量大）
    fft_result = fft2(large_mat);

    % 5. 大型矩阵的统计计算
    mat_stats = struct();
    mat_stats.mean_val = mean(large_mat(:));
    mat_stats.median_val = median(large_mat(:));
    mat_stats.std_val = std(large_mat(:));
    mat_stats.var_val = var(large_mat(:));
    mat_stats.min_val = min(large_mat(:));
    mat_stats.max_val = max(large_mat(:));

    % 6. 向量与矩阵的运算 - FIXED LINE 96!
    % 原始：vec_mat_result = real_param * mat_param + int_param;
    % 修复：确保所有操作数都是double类型
    vec_mat_result = double(real_param) * double(mat_param) + double(int_param);

    % 7. 基于布尔参数的条件计算
    if bool_param
        cond_result = sum(vec_param) * real_param;
    else
        cond_result = prod(vec_param) / real_param;
    end

    % 8. 结构体字段处理 - 增强兼容性
    struct_result = struct();
    if isfield(struct_param, 'field1')
        % 确保字段值是数值类型
        field1_val = struct_param.field1;
        if isnumeric(field1_val)
            struct_result.field1_plus = double(field1_val) + real_param;
        else
            struct_result.field1_plus = real_param; % 默认值
        end
    else
        struct_result.field1_plus = real_param; % 默认值
    end

    if isfield(struct_param, 'field2')
        field2_val = struct_param.field2;
        if isnumeric(field2_val)
            struct_result.field2_times = double(field2_val) * int_param;
        else
            struct_result.field2_times = int_param; % 默认值
        end
    else
        struct_result.field2_times = int_param; % 默认值
    end

    % 9. 元胞数组处理 - 增强兼容性
    cell_result = cell(size(cell_param));
    for i = 1:numel(cell_param)
        cell_item = cell_param{i};

        if isnumeric(cell_item)
            % 数值类型：统一转换为double后计算
            cell_result{i} = double(cell_item) + real_param;
        elseif ischar(cell_item) || isstring(cell_item)
            % 字符串类型：添加后缀
            if isstring(cell_item)
                cell_item = char(cell_item);
            end
            cell_result{i} = [cell_item, '_processed'];
        elseif islogical(cell_item)
            % 逻辑类型：取反
            cell_result{i} = ~cell_item;
        else
            % 其他类型：保持原样
            cell_result{i} = cell_item;
        end
    end

    % 10. 字符串处理
    str_result = upper(str_param); % 转换为大写

    % 11. 三维数组处理（包含较大计算量）
    % 三维数组缩放
    d3_scaled = real_param * d3_param;

    % 三维数组统计计算
    d3_stats = struct();
    d3_stats.mean_val = mean(d3_param(:));
    d3_stats.median_val = median(d3_param(:));
    d3_stats.std_val = std(d3_param(:));
    d3_stats.min_val = min(d3_param(:));
    d3_stats.max_val = max(d3_param(:));

    % 三维数组切片处理
    d3_slice = d3_param(:, :, 1) + d3_param(:, :, end);

    % 三维FFT变换（计算量大）
    d3_fft_result = fftn(d3_param);
    d3_fft_mean = mean(abs(d3_fft_result(:)));

    % ---------------------- 构建返回结构体 ----------------------
    result_struct = struct();

    % 基本类型结果 - 确保都是MATLAB标准类型
    result_struct.str_output = str_result;
    result_struct.real_output = real_param * 2;
    result_struct.int_output = int_param + 100;
    result_struct.bool_output = ~bool_param;

    % 数组类型结果
    result_struct.vec_output = vec_param * real_param;
    result_struct.mat_output = vec_mat_result;

    % 三维数组结果
    result_struct.d3_output = d3_scaled;
    result_struct.d3_stats = d3_stats;
    result_struct.d3_slice = d3_slice;
    result_struct.d3_fft_mean = d3_fft_mean;

    % 大型计算结果（提取关键统计信息，避免返回过大数据）
    result_struct.large_mat_stats = mat_stats;
    result_struct.fft_mean_abs = mean(abs(fft_result(:)));
    result_struct.lu_cond = cond(large_mat); % 矩阵条件数

    % 条件计算结果
    result_struct.cond_output = cond_result;

    % 结构体和元胞数组结果
    result_struct.struct_output = struct_result;
    result_struct.cell_output = cell_result;

    % 计算耗时信息
    result_struct.computation_time = toc; % 记录函数执行时间

    save('complex_matlab_func_result.mat','result_struct');
end


#endif

#endif // MYTHREAD_E_H
