#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include "serverwindow.h"
#include "clientwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Система уведомлений");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("PR3");
    
    // Парсер командной строки
    QCommandLineParser parser;
    parser.setApplicationDescription("Клиент-серверное приложение для системы уведомлений");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Опции
    QCommandLineOption serverOption(QStringList() << "s" << "server", "Запустить в режиме сервера");
    QCommandLineOption clientOption(QStringList() << "c" << "client", "Запустить в режиме клиента");
    QCommandLineOption dbHostOption("db-host", "Хост базы данных", "host", "localhost");
    QCommandLineOption dbNameOption("db-name", "Имя базы данных", "name", "notifications");
    QCommandLineOption dbUserOption("db-user", "Пользователь БД", "user", "postgres");
    QCommandLineOption dbPasswordOption("db-password", "Пароль БД", "password", "password");
    QCommandLineOption portOption("port", "Порт для подключения", "port", "12345");
    
    parser.addOption(serverOption);
    parser.addOption(clientOption);
    parser.addOption(dbHostOption);
    parser.addOption(dbNameOption);
    parser.addOption(dbUserOption);
    parser.addOption(dbPasswordOption);
    parser.addOption(portOption);
    
    parser.process(app);
    
    bool isServer = parser.isSet(serverOption);
    bool isClient = parser.isSet(clientOption);
    
    if (!isServer && !isClient) {
        // Показать диалог выбора режима
        QMessageBox msgBox;
        msgBox.setWindowTitle("Выбор режима");
        msgBox.setText("Выберите режим запуска приложения:");
        QPushButton *serverBtn = msgBox.addButton("Сервер", QMessageBox::ActionRole);
        QPushButton *clientBtn = msgBox.addButton("Клиент", QMessageBox::ActionRole);
        msgBox.addButton("Отмена", QMessageBox::RejectRole);
        
        msgBox.exec();
        
        if (msgBox.clickedButton() == serverBtn) {
            isServer = true;
        } else if (msgBox.clickedButton() == clientBtn) {
            isClient = true;
        } else {
            return 0;
        }
    }
    
    if (isServer && isClient) {
        QMessageBox::critical(nullptr, "Ошибка", "Нельзя запустить сервер и клиент одновременно!");
        return 1;
    }
    
    if (isServer) {
        qDebug() << "Запуск в режиме сервера";
        ServerWindow serverWindow;
        serverWindow.show();
        return app.exec();
    } else if (isClient) {
        qDebug() << "Запуск в режиме клиента";
        
        if (!QSystemTrayIcon::isSystemTrayAvailable()) {
            QMessageBox::critical(nullptr, "System Tray", 
                                "System tray недоступен в этой системе!");
        }
        
        ClientWindow clientWindow;
        clientWindow.show();
        return app.exec();
    }
    
    return 0;
}
