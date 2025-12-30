#include "facerecognitionmanager.h"
#include <QDir>
#include <QFile>
#include <QDebug>

FaceRecognitionManager::FaceRecognitionManager(QObject *parent)
    : QObject(parent)
    , m_dbManager(new DatabaseManager(this))
{
    initFaceRecognition();
}

FaceRecognitionManager::~FaceRecognitionManager()
{
    closeCamera();
}

bool FaceRecognitionManager::initFaceRecognition()
{
    // 加载人脸检测器
    QString cascadePath = "haarcascades/haarcascade_frontalface_default.xml";
    if (!m_faceCascade.load(cascadePath.toStdString())) {
        qDebug() << "Failed to load face cascade";
        return false;
    }

    // 初始化识别器
    m_recognizer = cv::face::LBPHFaceRecognizer::create();

    // 尝试加载现有模型
    QString modelPath = "face_model.yml";
    if (QFile::exists(modelPath)) {
        try {
            m_recognizer->read(modelPath.toStdString());
            // 加载标签映射
            // 这里需要从数据库加载标签映射
        } catch (const cv::Exception &e) {
            qDebug() << "Failed to load model:" << e.what();
        }
    }

    return true;
}

bool FaceRecognitionManager::registerFace(const QString &employeeId, const cv::Mat &faceImage)
{
    if (faceImage.empty()) {
        emit recognitionError("人脸图像为空");
        return false;
    }

    // 检测人脸
    std::vector<cv::Rect> faces = detectFaces(faceImage);
    if (faces.empty()) {
        emit recognitionError("未检测到人脸");
        return false;
    }

    // 提取人脸区域
    cv::Mat faceROI = faceImage(faces[0]).clone();
    cv::Mat processedFace = preprocessFace(faceROI);

    // 保存到数据库
    EmployeeInfo employee;
    employee.employeeId = employeeId;

    // 转换cv::Mat到QByteArray（特征数据）
    std::vector<uchar> buffer;
    cv::imencode(".png", processedFace, buffer);
    QByteArray faceData(reinterpret_cast<char*>(buffer.data()), buffer.size());
    employee.faceData = faceData;

    // 转换cv::Mat到QImage
    cv::Mat rgbMat;
    cv::cvtColor(faceROI, rgbMat, cv::COLOR_BGR2RGB);
    QImage faceImageQt(rgbMat.data, rgbMat.cols, rgbMat.rows,
                      rgbMat.step, QImage::Format_RGB888);
    employee.faceImage = faceImageQt.copy();

    // 保存到数据库
    if (!m_dbManager->addEmployee(employee)) {
        emit recognitionError("保存到数据库失败");
        return false;
    }

    // 重新训练模型
    return trainAllFaces();
}

RecognitionResult FaceRecognitionManager::recognizeFace(const cv::Mat &frame)
{
    RecognitionResult result;
    result.isValid = false;

    if (frame.empty() || m_recognizer.empty()) {
        return result;
    }

    // 检测人脸
    std::vector<cv::Rect> faces = detectFaces(frame);
    if (faces.empty()) {
        return result;
    }

    // 处理第一个人脸
    cv::Rect faceRect = faces[0];
    cv::Mat faceROI = frame(faceRect);
    cv::Mat processedFace = preprocessFace(faceROI);

    try {
        int predictedLabel = -1;
        double confidence = 0.0;

        // 进行识别
        m_recognizer->predict(processedFace, predictedLabel, confidence);

        // 根据置信度判断是否识别成功
        if (confidence < 80 && m_labelMap.contains(predictedLabel)) {  // 置信度阈值
            result.employeeId = m_labelMap[predictedLabel];
            result.confidence = confidence;
            result.faceRect = faceRect;
            result.isValid = true;

            // 从数据库获取员工姓名
            EmployeeInfo employee = m_dbManager->getEmployeeByEmployeeId(result.employeeId);
            if (employee.id != 0) {
                result.employeeName = employee.name;
            }

            emit faceDetected(result);
        }
    } catch (const cv::Exception &e) {
        qDebug() << "Recognition error:" << e.what();
        emit recognitionError(QString("识别错误: %1").arg(e.what()));
    }

    return result;
}

bool FaceRecognitionManager::trainAllFaces()
{
    // 从数据库获取所有员工的人脸数据
    QList<EmployeeInfo> employees = m_dbManager->getAllEmployees();
    if (employees.isEmpty()) {
        return false;
    }

    std::vector<cv::Mat> images;
    std::vector<int> labels;
    m_labelMap.clear();

    int label = 0;
    for (const EmployeeInfo &employee : employees) {
        if (!employee.faceData.isEmpty()) {
            // 转换QByteArray到cv::Mat
            std::vector<uchar> buffer(employee.faceData.begin(), employee.faceData.end());
            cv::Mat faceMat = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);

            if (!faceMat.empty()) {
                cv::Mat processedFace = preprocessFace(faceMat);
                images.push_back(processedFace);
                labels.push_back(label);
                m_labelMap[label] = employee.employeeId;
                label++;
            }
        }
    }

    if (images.empty()) {
        return false;
    }

    try {
        // 训练模型
        m_recognizer->train(images, labels);

        // 保存模型
        QString modelPath = "face_model.yml";
        m_recognizer->save(modelPath.toStdString());

        return true;
    } catch (const cv::Exception &e) {
        qDebug() << "Training error:" << e.what();
        emit recognitionError(QString("训练错误: %1").arg(e.what()));
        return false;
    }
}

std::vector<cv::Rect> FaceRecognitionManager::detectFaces(const cv::Mat &frame)
{
    std::vector<cv::Rect> faces;
    if (frame.empty()) {
        return faces;
    }

    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(grayFrame, grayFrame);

    m_faceCascade.detectMultiScale(
        grayFrame, faces,
        1.1, 3, 0,
        cv::Size(100, 100)
    );

    return faces;
}

cv::Mat FaceRecognitionManager::preprocessFace(const cv::Mat &faceImage)
{
    cv::Mat processed = faceImage.clone();

    // 转换为灰度图
    if (processed.channels() > 1) {
        cv::cvtColor(processed, processed, cv::COLOR_BGR2GRAY);
    }

    // 调整大小
    cv::resize(processed, processed, cv::Size(100, 100));

    // 直方图均衡化
    cv::equalizeHist(processed, processed);

    return processed;
}

bool FaceRecognitionManager::openCamera(int cameraId)
{
    if (m_camera.isOpened()) {
        m_camera.release();
    }

    return m_camera.open(cameraId);
}

void FaceRecognitionManager::closeCamera()
{
    if (m_camera.isOpened()) {
        m_camera.release();
    }
}

cv::Mat FaceRecognitionManager::captureFrame()
{
    cv::Mat frame;
    if (m_camera.isOpened()) {
        m_camera >> frame;
    }
    return frame;
}
