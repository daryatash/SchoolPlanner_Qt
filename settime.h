#ifndef SETTIME_H
#define SETTIME_H

#include <QDialog>
#include <QTime>
#include <QTimeEdit>
#include <QPushButton>

// Определение класса SetTime, который наследуется от QDialog
class SetTime : public QDialog
{
    Q_OBJECT  // Макрос Q_OBJECT для поддержки сигналов и слотов

public:
    SetTime(QWidget *parent = nullptr);  // Конструктор
    void accept();  // Метод для обработки нажатия кнопки "ОК"
    QTime getStartTime() const;  // Метод для получения времени начала урока
    QTime getEndTime() const;  // Метод для получения времени конца урока
    void setTime(const QString &time);  // Метод для установки времени в виджеты
    void clear();  // Метод для очистки виджетов от времени

private:
    QTimeEdit *startTimeEdit;  // Указатель на виджет для ввода времени начала урока
    QTimeEdit *endTimeEdit;  // Указатель на виджет для ввода времени конца урока
    QPushButton *okButton;  // Указатель на кнопку "ОК"
    QPushButton *cancelButton;  // Указатель на кнопку "Отмена"
};

#endif // SETTIME_H
