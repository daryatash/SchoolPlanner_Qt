#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QMainWindow>
#include <QTableWidget>
#include <QTimeEdit>
#include <QString>
#include <QPair>
#include <QDateTime>
#include <QTimer>
#include "addplans.h"

// Определение класса MainWindow, который наследуется от QMainWindow
class MainWindow : public QMainWindow
{
    Q_OBJECT  // Макрос Q_OBJECT для поддержки сигналов и слотов

public:
    MainWindow(QWidget *parent = nullptr);  // Конструктор
    ~MainWindow();  // Деструктор
    QTableWidget* getTableWidget() { return tableWidget; }  // Геттер для tableWidget
    void insertDataIntoDatabase(const QString& day, const QString& time, const QString& subject, const QString& homework, bool checked, const QString& row);  // Метод для вставки данных в базу данных
    QTableWidgetItem* findTableWidgetItem(const QString& day, const QString& time, const QString& subject, const QString& homework, bool checked, const QString& row);  // Метод для поиска элемента таблицы
    bool isModified(const QString& day, const QString& time, const QString& subject, const QString& homework);  // Метод для проверки, были ли изменены текст или состояние галочки

private slots:  // Приватные слоты
    QPair<QString, QString> loadDataFromDatabase(const QString& day, const QString& row);  // Слот для загрузки данных из базы данных
    QString loadSubjectFromDatabase(const QString& day, const QString& row);  // Слот для загрузки предмета из базы данных
    void loadTimeFromDatabase();  // Слот для загрузки времени из базы данных
    void onDayClicked(int column);  // Слот, который вызывается при клике на заголовок столбца таблицы
    void onCellClicked(int row, int column);  // Слот, который вызывается при клике на ячейку таблицы
    void onTimeChanged(const QTime &time);  // Слот, который вызывается при изменении времени
    void connectToDatabase();  // Слот для подключения к базе данных
    void saveDataToTextFile();  // Слот для сохранения данных в текстовый файл
    void updateWindowTitle();  // Слот для обновления заголовка окна

public slots:  // Публичные слоты
    void deleteRecord(int column, int row);  // Слот для удаления записи из базы данных

private:  // Приватные члены
    QTableWidget *tableWidget;  // Указатель на виджет таблицы
    QTimeEdit *timeEdit;  // Указатель на виджет редактирования времени
    QComboBox* weekComboBox;  // Указатель на выпадающий список недели
    AddPlans *addPlans;  // Указатель на объект AddPlans
    QDateTime startTime;  // Время начала
    QTimer *timer;  // Указатель на таймер
};

#endif // MAINWINDOW_H
