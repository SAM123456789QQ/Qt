#ifndef FACERECOGNITIONMANAGER_H
#define FACERECOGNITIONMANAGER_H

#include <QObject>
#include <QMap>

#include "sqlits3DateBase/databasemanager.h"
#include <opencv2/opencv.hpp>

struct RecognitionResult {
    QString employeeId;
    QString employeeName;
    double confidence;
    cv::Rect faceRect;
    bool isValid;
};

class FaceRecognitionManager : public QObject
{
    Q_OBJECT
public:
    explicit FaceRecognitionManager(QObject *parent = nullptr);
    ~FaceRecognitionManager();

    bool initFaceRecognition();

    // 人脸注册
    bool registerFace(const QString &employeeId, const cv::Mat &faceImage);

    // 人脸识别
    RecognitionResult recognizeFace(const cv::Mat &frame);

    // 批量训练
    bool trainAllFaces();

    // 摄像头操作
    bool openCamera(int cameraId = 0);
    void closeCamera();
    cv::Mat captureFrame();

    // 人脸检测
    std::vector<cv::Rect> detectFaces(const cv::Mat &frame);

signals:
    void faceDetected(const RecognitionResult &result);
    void recognitionError(const QString &error);

private:
    cv::VideoCapture m_camera;
    cv::Ptr<cv::face::LBPHFaceRecognizer> m_recognizer;
    cv::CascadeClassifier m_faceCascade;
    DatabaseManager *m_dbManager;

    QMap<int, QString> m_labelMap;  // 标签到员工ID的映射
    cv::Mat preprocessFace(const cv::Mat &faceImage);
};

#endif // FACERECOGNITIONMANAGER_H
