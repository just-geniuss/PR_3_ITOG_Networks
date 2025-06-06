#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QDateTime>
#include <QString>

class Database
{
public:
    Database();
    ~Database();
    
    bool connectToDatabase(const QString& host, const QString& dbName, 
                          const QString& user, const QString& password, int port = 5432);
    bool createTables();
    bool logMessage(const QString& message);
    void closeConnection();
    
    QString getLastError() const { return lastError; }
    
private:
    QSqlDatabase db;
    QString lastError;
    bool isConnected;
};

#endif // DATABASE_H
