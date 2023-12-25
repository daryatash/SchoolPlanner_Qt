#ifndef DAYPLANS_H
#define DAYPLANS_H

#include <QDialog>
#include <QPushButton>
#include <QTableWidget>
#include "mainwindow.h"

// Определение класса DayPlans, который наследуется от QDialog
class DayPlans : public QDialog
{
    Q_OBJECT  // Макрос Q_OBJECT для поддержки сигналов и слотов

public:
    explicit DayPlans(const QString &day, MainWindow *mainWindow, QWidget *parent = nullptr);  // Конструктор
    ~DayPlans();  // Деструктор

private slots:
    void saveTableToFile();  // Слот для сохранения таблицы в файл
    void okButtonClicked();  // Слот, вызываемый при нажатии кнопки "ОК"

private:
    QTableWidget *tableWidget;  // Указатель на виджет таблицы
    QPushButton *okButton;  // Указатель на кнопку "ОК"
    QPushButton *cancelButton;  // Указатель на кнопку "Отмена"
    MainWindow *mainWindow;  // Указатель на главное окно
    QString day;  // День
    QPushButton *saveToFileButton;  // Указатель на кнопку "Сохранить в файл"
};

#endif // DAYPLANS_H
