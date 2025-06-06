#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QStatusBar>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include "client.h"

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void toggleClient();
    void onClientStatusChanged(bool running);
    void onMessageReceived(const QString& message);
    void onClientError(const QString& error);
    void showWindow();

private:
    void setupUI();
    void setupConnections();
    void updateUI();
    
    // UI элементы
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    
    // Группа настроек клиента
    QGroupBox* clientGroup;
    QHBoxLayout* clientLayout;
    QSpinBox* portSpinBox;
    QPushButton* toggleClientBtn;
    QLabel* statusLabel;
    
    // Группа полученных сообщений
    QGroupBox* messagesGroup;
    QVBoxLayout* messagesLayout;
    QTextEdit* messagesTextEdit;
    QPushButton* clearMessagesBtn;
    
    // Клиент
    Client* client;
    bool clientRunning;
    bool isHiddenToTray;
};

#endif // CLIENTWINDOW_H
