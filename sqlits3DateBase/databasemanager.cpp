#include "databasemanager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QBuffer>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{
    initDatabase();
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::initDatabase()
{
    // 设置数据库路径
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dbPath);
    if (!dir.exists()) {
        dir.mkpath(dbPath);
    }

    dbPath += "/attendance.db";

    m_database = QSqlDatabase::addDatabase("QSQLITE", "attendance_connection");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qDebug() << "Failed to open database:" << m_database.lastError();
        return false;
    }

    return createTables();
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    // 创建员工表
    QString createEmployeeTable =
        "CREATE TABLE IF NOT EXISTS employees ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "department TEXT,"
        "employee_id TEXT UNIQUE NOT NULL,"
        "face_feature BLOB,"
        "face_image BLOB,"
        "register_time DATETIME DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createEmployeeTable)) {
        qDebug() << "Failed to create employees table:" << query.lastError();
        return false;
    }

    // 创建考勤记录表
    QString createAttendanceTable =
        "CREATE TABLE IF NOT EXISTS attendance_records ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "employee_id TEXT NOT NULL,"
        "employee_name TEXT,"
        "check_in_time DATETIME,"
        "check_out_time DATETIME,"
        "status TEXT DEFAULT '正常',"
        "FOREIGN KEY (employee_id) REFERENCES employees(employee_id)"
        ")";

    if (!query.exec(createAttendanceTable)) {
        qDebug() << "Failed to create attendance table:" << query.lastError();
        return false;
    }

    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_employee_id ON employees(employee_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_attendance_date ON attendance_records(date(check_in_time))");

    return true;
}

bool DatabaseManager::addEmployee(const EmployeeInfo &employee)
{
    QSqlQuery query(m_database);
    query.prepare(
        "INSERT INTO employees (name, department, employee_id, face_feature, face_image) "
        "VALUES (:name, :department, :employee_id, :face_feature, :face_image)"
    );

    query.bindValue(":name", employee.name);
    query.bindValue(":department", employee.department);
    query.bindValue(":employee_id", employee.employeeId);

    // 转换人脸特征数据
    query.bindValue(":face_feature", employee.faceData);

    // 转换人脸图片
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    employee.faceImage.save(&buffer, "PNG");
    query.bindValue(":face_image", imageData);

    if (!query.exec()) {
        qDebug() << "Failed to add employee:" << query.lastError();
        return false;
    }

    return true;
}

QByteArray DatabaseManager::loadFaceFeature(const QString &employeeId)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT face_feature FROM employees WHERE employee_id = :employee_id");
    query.bindValue(":employee_id", employeeId);

    if (query.exec() && query.next()) {
        return query.value(0).toByteArray();
    }

    return QByteArray();
}

bool DatabaseManager::addCheckInRecord(const QString &employeeId, const QDateTime &time)
{
    // 先获取员工信息
    EmployeeInfo employee = getEmployeeByEmployeeId(employeeId);
    if (employee.id == 0) {
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare(
        "INSERT INTO attendance_records (employee_id, employee_name, check_in_time) "
        "VALUES (:employee_id, :employee_name, :check_in_time)"
    );

    query.bindValue(":employee_id", employeeId);
    query.bindValue(":employee_name", employee.name);
    query.bindValue(":check_in_time", time.toString("yyyy-MM-dd hh:mm:ss"));

    // 判断是否迟到（假设9:00前为正常）
    QTime checkTime = time.time();
    if (checkTime > QTime(9, 0, 0)) {
        query.bindValue(":status", "迟到");
    }

    if (!query.exec()) {
        qDebug() << "Failed to add check-in record:" << query.lastError();
        return false;
    }

    return true;
}

// 其他数据库操作函数...
