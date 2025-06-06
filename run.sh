#!/bin/bash

# Скрипт для запуска системы уведомлений
# Использование: ./run.sh [server|client]

# Добавляем Qt в PATH
export PATH="/opt/homebrew/opt/qt/bin:$PATH"

cd "$(dirname "$0")"

# Проверяем, скомпилировано ли приложение
if [ ! -f "PR3.app/Contents/MacOS/PR3" ]; then
    echo "Приложение не скомпилировано. Компилируем..."
    qmake PR3.pro && make
    
    if [ $? -ne 0 ]; then
        echo "Ошибка компиляции!"
        exit 1
    fi
fi

# Запускаем приложение с параметрами
if [ "$1" = "server" ]; then
    echo "Запуск сервера..."
    ./PR3.app/Contents/MacOS/PR3 --server
elif [ "$1" = "client" ]; then
    echo "Запуск клиента..."
    ./PR3.app/Contents/MacOS/PR3 --client
else
    echo "Запуск с выбором режима..."
    ./PR3.app/Contents/MacOS/PR3
fi
