#include <QCoreApplication>
#include <QDebug>
#include <QException>
#include "studentlist.h"

void printStudent(const Student& s) {
    qDebug() << "Шифр:" << s.shifr << "| ФИО:" << s.fio << "| Группа:" << s.group;
}

void testStudentList() {
    StudentList list;

    // Добавление студентов
    list.add({100, "Иванов Иван Иванович", "Группа 101"});
    list.add({200, "Петров Петр Петрович", "Группа 102"});
    list.add({300, "Сидорова Анна Михайловна", "Группа 103"});
    qDebug() << "Студенты добавлены";


    // Проверка поиска существующего студента

    printStudent(list.at(200));

    // Удаление студента
    qDebug() << "\nУдаление студента с шифром 100:";

    list.remove(100);


    // Вывод оставшихся студентов
    qDebug() << "\nТекущий список студентов:";
    for (int i = 0; i < list.size(); ++i) {
        printStudent(list.at_index(i));
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    try {
        testStudentList();
    } catch (const std::exception& e) {
        qDebug() << "Необработанная ошибка:" << e.what();
    } catch (...) {
        qDebug() << "Неизвестная ошибка";
    }

    return 0;
}
