#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QApplication>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    
    bool startClient(int port = 12346);
    void stopClient();
    bool isRunning() const { return clientRunning; }
    QString getLastError() const { return lastError; }
    
    void showTrayMessage(const QString& title, const QString& message);

signals:
    void messageReceived(const QString& message);
    void clientStatusChanged(bool running);
    void errorOccurred(const QString& error);
    void showWindowRequested();

private slots:
    void readPendingDatagrams();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showClientWindow();
    void quitApplication();

private:
    QUdpSocket* udpSocket;
    QSystemTrayIcon* trayIcon;
    QMenu* trayMenu;
    QAction* showAction;
    QAction* quitAction;
    
    int clientPort;
    bool clientRunning;
    QString lastError;
    
    void setupSystemTray();
};

#endif // CLIENT_H
