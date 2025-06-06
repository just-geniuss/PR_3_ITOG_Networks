#include "serverwindow.h"
#include <QApplication>
#include <QDateTime>

ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent), server(nullptr), serverRunning(false)
{
    server = new Server(this);
    setupUI();
    setupConnections();
    updateUI();
    
    setWindowTitle("Сервер системы уведомлений");
    setMinimumSize(800, 600);
    resize(1000, 700);
}

ServerWindow::~ServerWindow()
{
    if (server && server->isRunning()) {
        server->stopServer();
    }
}

void ServerWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    splitter = new QSplitter(Qt::Vertical, this);
    mainLayout->addWidget(splitter);
    
    // Группа настроек сервера
    serverGroup = new QGroupBox("Настройки сервера", this);
    serverLayout = new QGridLayout(serverGroup);
    
    serverLayout->addWidget(new QLabel("Порт:"), 0, 0);
    portSpinBox = new QSpinBox();
    portSpinBox->setRange(1024, 65535);
    portSpinBox->setValue(12345);
    serverLayout->addWidget(portSpinBox, 0, 1);
    
    serverLayout->addWidget(new QLabel("Имя файла БД:"), 1, 0);
    dbNameEdit = new QLineEdit("notifications");
    serverLayout->addWidget(dbNameEdit, 1, 1);
    
    serverLayout->addWidget(new QLabel("(Для SQLite)"), 2, 0);
    QLabel* infoLabel = new QLabel("Остальные поля игнорируются для SQLite");
    infoLabel->setStyleSheet("QLabel { color: gray; font-style: italic; }");
    serverLayout->addWidget(infoLabel, 2, 1);
    
    toggleServerBtn = new QPushButton("Запустить сервер");
    toggleServerBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    serverLayout->addWidget(toggleServerBtn, 3, 0, 1, 2);
    
    statusLabel = new QLabel("Статус: Остановлен");
    statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    serverLayout->addWidget(statusLabel, 4, 0, 1, 2);
    
    splitter->addWidget(serverGroup);
    
    // Группа отправки сообщений
    messageGroup = new QGroupBox("Отправка уведомлений", this);
    messageLayout = new QVBoxLayout(messageGroup);
    
    messageEdit = new QLineEdit();
    messageEdit->setPlaceholderText("Введите сообщение для отправки всем клиентам...");
    messageEdit->setStyleSheet("QLineEdit { padding: 8px; font-size: 12px; }");
    messageLayout->addWidget(messageEdit);
    
    sendBtn = new QPushButton("Отправить уведомление");
    sendBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px; }");
    sendBtn->setEnabled(false);
    messageLayout->addWidget(sendBtn);
    
    splitter->addWidget(messageGroup);
    
    // Группа лога
    logGroup = new QGroupBox("Журнал сообщений", this);
    logLayout = new QVBoxLayout(logGroup);
    
    logTextEdit = new QTextEdit();
    logTextEdit->setReadOnly(true);
    logTextEdit->setStyleSheet("QTextEdit { background-color: #f5f5f5; font-family: monospace; }");
    logLayout->addWidget(logTextEdit);
    
    clearLogBtn = new QPushButton("Очистить журнал");
    clearLogBtn->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 5px; }");
    logLayout->addWidget(clearLogBtn);
    
    splitter->addWidget(logGroup);
    
    // Настройка пропорций splitter
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 0);
    splitter->setStretchFactor(2, 1);
    
    // Статусная строка
    statusBar()->showMessage("Готов к работе");
}

void ServerWindow::setupConnections()
{
    connect(toggleServerBtn, &QPushButton::clicked, this, &ServerWindow::toggleServer);
    connect(sendBtn, &QPushButton::clicked, this, &ServerWindow::sendMessage);
    connect(clearLogBtn, &QPushButton::clicked, logTextEdit, &QTextEdit::clear);
    connect(messageEdit, &QLineEdit::returnPressed, this, &ServerWindow::sendMessage);
    
    connect(server, &Server::serverStatusChanged, this, &ServerWindow::onServerStatusChanged);
    connect(server, &Server::messageLogged, this, &ServerWindow::onMessageLogged);
    connect(server, &Server::errorOccurred, this, &ServerWindow::onServerError);
}

void ServerWindow::toggleServer()
{
    if (!serverRunning) {
        // Запуск сервера
        bool success = server->startServer(
            portSpinBox->value(),
            "", // host не используется для SQLite
            dbNameEdit->text(),
            "", // user не используется для SQLite  
            ""  // password не используется для SQLite
        );
        
        if (success) {
            statusBar()->showMessage("Сервер запущен на IP: " + server->getLocalIP());
        }
    } else {
        // Остановка сервера
        server->stopServer();
        statusBar()->showMessage("Сервер остановлен");
    }
}

void ServerWindow::sendMessage()
{
    if (!serverRunning) {
        QMessageBox::warning(this, "Ошибка", "Сервер не запущен!");
        return;
    }
    
    QString message = messageEdit->text().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите сообщение для отправки!");
        return;
    }
    
    if (server->sendBroadcastMessage(message)) {
        messageEdit->clear();
        messageEdit->setFocus();
    }
}

void ServerWindow::onServerStatusChanged(bool running)
{
    serverRunning = running;
    updateUI();
}

void ServerWindow::onMessageLogged(const QString& message)
{
    QString logEntry = QString("[%1] ОТПРАВЛЕНО: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(message);
    
    logTextEdit->append(logEntry);
    logTextEdit->ensureCursorVisible();
}

void ServerWindow::onServerError(const QString& error)
{
    QMessageBox::critical(this, "Ошибка сервера", error);
    statusBar()->showMessage("Ошибка: " + error, 5000);
    
    QString logEntry = QString("[%1] ОШИБКА: %2")
                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      .arg(error);
    
    logTextEdit->append(logEntry);
}

void ServerWindow::updateUI()
{
    if (serverRunning) {
        toggleServerBtn->setText("Остановить сервер");
        toggleServerBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
        statusLabel->setText("Статус: Запущен");
        statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        sendBtn->setEnabled(true);
        
        // Отключение полей настроек
        portSpinBox->setEnabled(false);
        dbNameEdit->setEnabled(false);
    } else {
        toggleServerBtn->setText("Запустить сервер");
        toggleServerBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
        statusLabel->setText("Статус: Остановлен");
        statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        sendBtn->setEnabled(false);
        
        // Включение полей настроек
        portSpinBox->setEnabled(true);
        dbNameEdit->setEnabled(true);
    }
}
