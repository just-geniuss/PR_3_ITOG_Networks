#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QStatusBar>
#include <QMessageBox>
#include <QSplitter>
#include "server.h"

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();

private slots:
    void toggleServer();
    void sendMessage();
    void onServerStatusChanged(bool running);
    void onMessageLogged(const QString& message);
    void onServerError(const QString& error);

private:
    void setupUI();
    void setupConnections();
    void updateUI();
    
    // UI элементы
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    QSplitter* splitter;
    
    // Группа настроек сервера
    QGroupBox* serverGroup;
    QGridLayout* serverLayout;
    QSpinBox* portSpinBox;
    QLineEdit* dbNameEdit;
    QPushButton* toggleServerBtn;
    QLabel* statusLabel;
    
    // Группа отправки сообщений
    QGroupBox* messageGroup;
    QVBoxLayout* messageLayout;
    QLineEdit* messageEdit;
    QPushButton* sendBtn;
    
    // Лог сообщений
    QGroupBox* logGroup;
    QVBoxLayout* logLayout;
    QTextEdit* logTextEdit;
    QPushButton* clearLogBtn;
    
    // Сервер
    Server* server;
    bool serverRunning;
};

#endif // SERVERWINDOW_H
