#include "server.h"
#include <QDebug>
#include <QDateTime>

Server::Server(QObject *parent)
    : QObject(parent), udpSocket(nullptr), database(nullptr), 
      serverPort(12345), serverRunning(false)
{
    database = new Database();
}

Server::~Server()
{
    stopServer();
    if (database) {
        delete database;
    }
}

bool Server::startServer(int port, const QString& dbHost, const QString& dbName, 
                       const QString& dbUser, const QString& dbPassword)
{
    if (serverRunning) {
        stopServer();
    }
    
    serverPort = port;
    
    // Подключение к базе данных
    if (!database->connectToDatabase(dbHost, dbName, dbUser, dbPassword)) {
        lastError = "Ошибка подключения к БД: " + database->getLastError();
        emit errorOccurred(lastError);
        return false;
    }
    
    // Создание UDP сокета
    udpSocket = new QUdpSocket(this);
    
    if (!udpSocket->bind(QHostAddress::Any, serverPort)) {
        lastError = "Не удалось привязать сокет к порту " + QString::number(serverPort);
        emit errorOccurred(lastError);
        delete udpSocket;
        udpSocket = nullptr;
        return false;
    }
    
    connect(udpSocket, &QUdpSocket::readyRead, this, &Server::readPendingDatagrams);
    
    serverRunning = true;
    emit serverStatusChanged(true);
    
    qDebug() << "Сервер запущен на порту" << serverPort;
    qDebug() << "IP адрес сервера:" << getLocalIP();
    
    return true;
}

void Server::stopServer()
{
    if (!serverRunning) return;
    
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
        udpSocket = nullptr;
    }
    
    if (database) {
        database->closeConnection();
    }
    
    serverRunning = false;
    emit serverStatusChanged(false);
    
    qDebug() << "Сервер остановлен";
}

bool Server::sendBroadcastMessage(const QString& message)
{
    if (!serverRunning || !udpSocket) {
        lastError = "Сервер не запущен";
        emit errorOccurred(lastError);
        return false;
    }
    
    QString formattedMessage = QString("[%1] %2")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                              .arg(message);
    
    QByteArray data = formattedMessage.toUtf8();
    
    // Отправка на broadcast адрес
    QHostAddress broadcastAddress = QHostAddress::Broadcast;
    qint64 result = udpSocket->writeDatagram(data, broadcastAddress, serverPort + 1);
    
    if (result == -1) {
        lastError = "Ошибка отправки сообщения";
        emit errorOccurred(lastError);
        return false;
    }
    
    // Журналирование в БД
    if (!database->logMessage(message)) {
        qDebug() << "Предупреждение: не удалось записать сообщение в БД";
    }
    
    emit messageLogged(formattedMessage);
    qDebug() << "Отправлено broadcast сообщение:" << formattedMessage;
    
    return true;
}

QString Server::getLocalIP() const
{
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (const QHostAddress& address : addresses) {
        if (address != QHostAddress::LocalHost && 
            address.toIPv4Address() && 
            !address.isLoopback()) {
            return address.toString();
        }
    }
    return "127.0.0.1";
}

void Server::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        
        udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        
        qDebug() << "Получен запрос от клиента:" << sender.toString() << ":" << senderPort;
    }
}
