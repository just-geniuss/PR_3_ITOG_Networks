#include "client.h"
#include <QDebug>
#include <QDateTime>
#include <QIcon>
#include <QStyle>

Client::Client(QObject *parent)
    : QObject(parent), udpSocket(nullptr), trayIcon(nullptr), 
      clientPort(12346), clientRunning(false)
{
    setupSystemTray();
}

Client::~Client()
{
    stopClient();
}

bool Client::startClient(int port)
{
    if (clientRunning) {
        stopClient();
    }
    
    clientPort = port;
    
    // Создание UDP сокета
    udpSocket = new QUdpSocket(this);
    
    if (!udpSocket->bind(QHostAddress::Any, clientPort)) {
        lastError = "Не удалось привязать клиентский сокет к порту " + QString::number(clientPort);
        emit errorOccurred(lastError);
        delete udpSocket;
        udpSocket = nullptr;
        return false;
    }
    
    connect(udpSocket, &QUdpSocket::readyRead, this, &Client::readPendingDatagrams);
    
    clientRunning = true;
    emit clientStatusChanged(true);
    
    qDebug() << "Клиент запущен и слушает порт" << clientPort;
    
    return true;
}

void Client::stopClient()
{
    if (!clientRunning) return;
    
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
        udpSocket = nullptr;
    }
    
    clientRunning = false;
    emit clientStatusChanged(false);
    
    qDebug() << "Клиент остановлен";
}

void Client::showTrayMessage(const QString& title, const QString& message)
{
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 5000);
    }
}

void Client::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        
        udpSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        
        QString message = QString::fromUtf8(buffer);
        
        qDebug() << "Получено сообщение от сервера:" << message;
        
        // Показать сообщение в system tray
        showTrayMessage("Новое уведомление", message);
        
        emit messageReceived(message);
    }
}

void Client::setupSystemTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << "System tray недоступен в этой системе";
        return;
    }
    
    // Создание иконки (используем стандартную иконку)
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    
    trayIcon = new QSystemTrayIcon(icon, this);
    
    // Создание контекстного меню
    trayMenu = new QMenu();
    
    showAction = new QAction("Показать клиент", this);
    connect(showAction, &QAction::triggered, this, &Client::showClientWindow);
    
    quitAction = new QAction("Выход", this);
    connect(quitAction, &QAction::triggered, this, &Client::quitApplication);
    
    trayMenu->addAction(showAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);
    
    trayIcon->setContextMenu(trayMenu);
    
    connect(trayIcon, &QSystemTrayIcon::activated, this, &Client::trayIconActivated);
    
    trayIcon->setToolTip("Клиент системы уведомлений");
    trayIcon->show();
    
    qDebug() << "System tray настроен";
}

void Client::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        showClientWindow();
    }
}

void Client::showClientWindow()
{
    emit showWindowRequested();
}

void Client::quitApplication()
{
    stopClient();
    QApplication::quit();
}
