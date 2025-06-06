#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QNetworkInterface>
#include "database.h"

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    
    bool startServer(int port = 12345, const QString& dbHost = "localhost", 
                    const QString& dbName = "notifications", 
                    const QString& dbUser = "postgres", 
                    const QString& dbPassword = "password");
    void stopServer();
    bool sendBroadcastMessage(const QString& message);
    
    QString getLocalIP() const;
    bool isRunning() const { return serverRunning; }
    QString getLastError() const { return lastError; }

signals:
    void messageLogged(const QString& message);
    void serverStatusChanged(bool running);
    void errorOccurred(const QString& error);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket* udpSocket;
    Database* database;
    int serverPort;
    bool serverRunning;
    QString lastError;
    
    QList<QHostAddress> getActiveClients();
};

#endif // SERVER_H
