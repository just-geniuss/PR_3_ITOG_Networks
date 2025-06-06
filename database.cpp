#include "database.h"
#include <QDebug>

Database::Database() : isConnected(false)
{
    // Используем SQLite вместо PostgreSQL для простоты демонстрации
    db = QSqlDatabase::addDatabase("QSQLITE");
}

Database::~Database()
{
    if (isConnected) {
        closeConnection();
    }
}

bool Database::connectToDatabase(const QString& host, const QString& dbName, 
                               const QString& user, const QString& password, int port)
{
    // Для SQLite используем только имя файла базы данных
    QString dbPath = dbName + ".db";
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        lastError = db.lastError().text();
        qDebug() << "Ошибка подключения к БД:" << lastError;
        return false;
    }
    
    isConnected = true;
    qDebug() << "Успешное подключение к БД SQLite:" << dbPath;
    return createTables();
}

bool Database::createTables()
{
    QSqlQuery query;
    QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            message TEXT NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(createTableQuery)) {
        lastError = query.lastError().text();
        qDebug() << "Ошибка создания таблицы:" << lastError;
        return false;
    }
    
    qDebug() << "Таблица messages создана или уже существует";
    return true;
}

bool Database::logMessage(const QString& message)
{
    if (!isConnected) {
        lastError = "Нет подключения к БД";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO messages (message, timestamp) VALUES (:message, :timestamp)");
    query.bindValue(":message", message);
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    
    if (!query.exec()) {
        lastError = query.lastError().text();
        qDebug() << "Ошибка записи сообщения в БД:" << lastError;
        return false;
    }
    
    qDebug() << "Сообщение записано в БД:" << message;
    return true;
}

void Database::closeConnection()
{
    if (isConnected) {
        db.close();
        isConnected = false;
        qDebug() << "Соединение с БД закрыто";
    }
}
