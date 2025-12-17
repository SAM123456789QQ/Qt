#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QImage>
#include <QPixmap>

struct EmployeeInfo {
    int id;
    QString name;
    QString department;
    QString employeeId;
    QByteArray faceData;  // 人脸特征数据
    QImage faceImage;     // 人脸照片
    QDateTime registerTime;
};

struct AttendanceRecord {
    int id;
    QString employeeId;
    QString employeeName;
    QDateTime checkInTime;
    QDateTime checkOutTime;
    QString status;  // 正常、迟到、早退等
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initDatabase();

    // 员工管理
    bool addEmployee(const EmployeeInfo &employee);
    bool updateEmployee(const EmployeeInfo &employee);
    bool deleteEmployee(int id);
    QList<EmployeeInfo> getAllEmployees();
    EmployeeInfo getEmployeeById(int id);
    EmployeeInfo getEmployeeByEmployeeId(const QString &employeeId);

    // 人脸特征管理
    bool saveFaceFeature(const QString &employeeId, const QByteArray &feature);
    QByteArray loadFaceFeature(const QString &employeeId);

    // 考勤记录
    bool addCheckInRecord(const QString &employeeId, const QDateTime &time);
    bool addCheckOutRecord(const QString &employeeId, const QDateTime &time);
    bool updateAttendanceRecord(const AttendanceRecord &record);
    QList<AttendanceRecord> getAttendanceRecords(const QDate &date);
    QList<AttendanceRecord> getEmployeeAttendance(const QString &employeeId,
                                                  const QDate &startDate,
                                                  const QDate &endDate);

    // 统计
    int getTodayAttendanceCount();
    QMap<QString, int> getAttendanceStatistics(const QDate &date);

private:
    QSqlDatabase m_database;
    bool createTables();
};

#endif // DATABASEMANAGER_H
