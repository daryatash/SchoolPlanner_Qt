#ifndef ADDPLANS_H
#define ADDPLANS_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QColor>
#include <QTimer>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QMessageBox>

// Определение класса AddPlans, который наследуется от QDialog
class AddPlans : public QDialog
{
    Q_OBJECT  // Макрос Q_OBJECT для поддержки сигналов и слотов

public:
    AddPlans(QWidget *parent = nullptr);  // Конструктор
    QString getSubject() const;  // Метод для получения предмета
    QString getHomework() const;  // Метод для получения домашнего задания
    void setData(const QString &subject, const QString &homework);  // Метод для установки данных
    void clear();  // Метод для очистки полей ввода
    int currentRow;  // Текущая строка
    int currentColumn;  // Текущий столбец
    void setColumn(int column) { this->column = column; }  // Метод для установки столбца
    void setRow(int row) { this->row = row; }  // Метод для установки строки

public slots:
    void saveAndAccept();  // Слот для сохранения данных и закрытия окна
    void checkModified();  // Слот для проверки изменения данных
    void deleteAndClear();  // Слот для удаления записи и очистки полей ввода

signals:
    void colorChanged(QColor color);  // Сигнал для изменения цвета
    void deleteRecordRequested(int column, int row);  // Сигнал для удаления записи

private slots:
    void setNotification();  // Слот для установки напоминания
    void checkNotification();  // Слот для проверки напоминания

private:
    QLineEdit *subjectLineEdit;  // Указатель на виджет для ввода предмета
    QTextEdit *homeworkTextEdit;  // Указатель на виджет для ввода домашнего задания
    QPushButton *okButton;  // Указатель на кнопку "ОК"
    QPushButton *cancelButton;  // Указатель на кнопку "Отмена"
    bool isModified;  // Флаг изменения данных
    QString originalSubject;  // Оригинальное название предмета
    QString originalHomework;  // Оригинальное домашнее задание
    QDateTimeEdit *dateTimeEdit;  // Указатель на виджет для ввода даты и времени
    QTimer *notificationTimer;  // Указатель на таймер для напоминаний
    QPushButton *setNotificationButton;  // Указатель на кнопку "Напомнить"
    QPushButton *deleteButton;  // Указатель на кнопку "Удалить"
    QDateTime notificationTime;  // Время напоминания
    struct Notification {  // Структура для хранения информации о напоминании
        QDateTime time;  // Время напоминания
        QString subject;  // Предмет
        QString homework;  // Домашнее задание
    };
    QList<Notification> notifications;  // Список напоминаний
    int column;  // Столбец
    int row;  // Строка
};

#endif // ADDPLANS_H
