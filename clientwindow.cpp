#include "clientwindow.h"
#include <QApplication>
#include <QDateTime>

ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent), client(nullptr), clientRunning(false), isHiddenToTray(false)
{
    client = new Client(this);
    setupUI();
    setupConnections();
    updateUI();
    
    setWindowTitle("Клиент системы уведомлений");
    setMinimumSize(600, 500);
    resize(800, 600);
}

ClientWindow::~ClientWindow()
{
    if (client && client->isRunning()) {
        client->stopClient();
    }
}

void ClientWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Группа настроек клиента
    clientGroup = new QGroupBox("Настройки клиента", this);
    clientLayout = new QHBoxLayout(clientGroup);
    
    clientLayout->addWidget(new QLabel("Порт:"));
    portSpinBox = new QSpinBox();
    portSpinBox->setRange(1024, 65535);
    portSpinBox->setValue(12346);
    clientLayout->addWidget(portSpinBox);
    
    clientLayout->addStretch();
    
    toggleClientBtn = new QPushButton("Запустить клиент");
    toggleClientBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    clientLayout->addWidget(toggleClientBtn);
    
    statusLabel = new QLabel("Статус: Остановлен");
    statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    clientLayout->addWidget(statusLabel);
    
    mainLayout->addWidget(clientGroup);
    
    // Группа полученных сообщений
    messagesGroup = new QGroupBox("Полученные уведомления", this);
    messagesLayout = new QVBoxLayout(messagesGroup);
    
    messagesTextEdit = new QTextEdit();
    messagesTextEdit->setReadOnly(true);
    messagesTextEdit->setStyleSheet("QTextEdit { background-color: #f5f5f5; font-family: monospace; }");
    messagesTextEdit->setPlaceholderText("Полученные уведомления будут отображаться здесь...");
    messagesLayout->addWidget(messagesTextEdit);
    
    clearMessagesBtn = new QPushButton("Очистить сообщения");
    clearMessagesBtn->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 5px; }");
    messagesLayout->addWidget(clearMessagesBtn);
    
    mainLayout->addWidget(messagesGroup);
    
    // Статусная строка
    statusBar()->showMessage("Готов к работе");
}

void ClientWindow::setupConnections()
{
    connect(toggleClientBtn, &QPushButton::clicked, this, &ClientWindow::toggleClient);
    connect(clearMessagesBtn, &QPushButton::clicked, messagesTextEdit, &QTextEdit::clear);
    
    connect(client, &Client::clientStatusChanged, this, &ClientWindow::onClientStatusChanged);
    connect(client, &Client::messageReceived, this, &ClientWindow::onMessageReceived);
    connect(client, &Client::errorOccurred, this, &ClientWindow::onClientError);
    connect(client, &Client::showWindowRequested, this, &ClientWindow::showWindow);
}

void ClientWindow::toggleClient()
{
    if (!clientRunning) {
        // Запуск клиента
        bool success = client->startClient(portSpinBox->value());
        
        if (success) {
            statusBar()->showMessage("Клиент запущен и слушает порт " + QString::number(portSpinBox->value()));
        }
    } else {
        // Остановка клиента
        client->stopClient();
        statusBar()->showMessage("Клиент остановлен");
    }
}

void ClientWindow::onClientStatusChanged(bool running)
{
    clientRunning = running;
    updateUI();
}

void ClientWindow::onMessageReceived(const QString& message)
{
    QString logEntry = QString("[%1] ПОЛУЧЕНО: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(message);
    
    messagesTextEdit->append(logEntry);
    messagesTextEdit->ensureCursorVisible();
    
    // Если окно свернуто, показать иконку в трее
    if (isHiddenToTray) {
        client->showTrayMessage("Новое уведомление", message);
    }
}

void ClientWindow::onClientError(const QString& error)
{
    QMessageBox::critical(this, "Ошибка клиента", error);
    statusBar()->showMessage("Ошибка: " + error, 5000);
    
    QString logEntry = QString("[%1] ОШИБКА: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(error);
    
    messagesTextEdit->append(logEntry);
}

void ClientWindow::updateUI()
{
    if (clientRunning) {
        toggleClientBtn->setText("Остановить клиент");
        toggleClientBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
        statusLabel->setText("Статус: Запущен");
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        
        // Отключение поля порта
        portSpinBox->setEnabled(false);
    } else {
        toggleClientBtn->setText("Запустить клиент");
        toggleClientBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
        statusLabel->setText("Статус: Остановлен");
        statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        
        // Включение поля порта
        portSpinBox->setEnabled(true);
    }
}

void ClientWindow::closeEvent(QCloseEvent *event)
{
    if (clientRunning && QSystemTrayIcon::isSystemTrayAvailable()) {
        hide();
        isHiddenToTray = true;
        client->showTrayMessage("Клиент системы уведомлений", 
                               "Приложение свернуто в системный трей. Уведомления будут отображаться здесь.");
        event->ignore();
    } else {
        event->accept();
    }
}

void ClientWindow::showWindow()
{
    if (isHiddenToTray) {
        show();
        raise();
        activateWindow();
        isHiddenToTray = false;
    }
}
