#include "mainwindow.h"
#include "addplans.h"
#include "settime.h"
#include "dayplans.h"
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QTableWidget>
#include <QSettings>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)  // Конструктор класса MainWindow
{
    // Создание нового виджета QTableWidget с 7 строками и 8 столбцами
    tableWidget = new QTableWidget(7, 8, this);

    // Установка режима редактирования ячеек таблицы (запрет на редактирование)
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Установка режима изменения размера для горизонтальных и вертикальных заголовков
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Установка названий столбцов таблицы
    QStringList daysOfWeek = {"Time", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    tableWidget->setHorizontalHeaderLabels(daysOfWeek);

    // Подключение сигнала cellClicked к слоту onCellClicked
    connect(tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(onCellClicked(int,int)));

    // Установка tableWidget в качестве центрального виджета
    setCentralWidget(tableWidget);

    // Подключение к базе данных
    connectToDatabase();

    // Создание нового объекта addPlans и подключение сигнала deleteRecordRequested к слоту deleteRecord
    addPlans = new AddPlans(this);
    connect(addPlans, SIGNAL(deleteRecordRequested(int,int)), this, SLOT(deleteRecord(int,int)));

    // Подключение сигнала sectionClicked к слоту onDayClicked
    connect(tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onDayClicked(int)));

    // Загрузка времени из базы данных
    loadTimeFromDatabase();

    // Заполнение ячеек таблицы данными из базы данных
    int rows = tableWidget->rowCount();
    int columns = tableWidget->columnCount();
    for(int i = 0; i < rows; ++i) {
        for(int j = 1; j < columns; ++j) {
            QString day = tableWidget->horizontalHeaderItem(j)->text();
            QString subject = loadSubjectFromDatabase(day, QString::number(i));
            if(!subject.isEmpty()) {
                QTableWidgetItem *item = new QTableWidgetItem(subject);
                tableWidget->setItem(i, j, item);
            }
        }
    }

    // Установка текущего времени
    startTime = QDateTime::currentDateTime();

    // Создание нового таймера и подключение сигнала timeout к слоту updateWindowTitle
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateWindowTitle()));
    timer->start(1000);

    // Отображение окна на весь экран
    showMaximized();
}

MainWindow::~MainWindow()  // Деструктор класса MainWindow
{
}

void MainWindow::connectToDatabase()  // Метод для подключения к базе данных
{
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("database.db");
    if (!database.open()) {
        qDebug() << "Failed to connect to the database";
    } else {
        qDebug() << "Connected to the database";
    }
}


void MainWindow::insertDataIntoDatabase(const QString& day, const QString& time, const QString& subject, const QString& homework, bool checked, const QString& row)
{
    QSqlQuery query;  // Создание объекта QSqlQuery

    // Подготовка запроса на выборку из базы данных
    query.prepare("SELECT 1 FROM database WHERE day = :day AND time = :time AND row = :row");
    query.bindValue(":day", day);
    query.bindValue(":time", time);
    query.bindValue(":row", row);

    if (query.exec()) {  // Выполнение запроса
        if (query.first() > 0) {  // Если запись существует
            // Подготовка запроса на обновление записи в базе данных
            query.prepare("UPDATE database SET subject = :subject, homework = :homework, checked = :checked WHERE day = :day AND time = :time AND row = :row");
            query.bindValue(":day", day);
            query.bindValue(":time", time);
            query.bindValue(":subject", subject);
            query.bindValue(":homework", homework);
            query.bindValue(":checked", checked ? 1 : 0);
            query.bindValue(":row", row);
            if (!query.exec()) {  // Если запрос не выполнен
                qDebug() << "Failed to update data in the database";
                qDebug() << "Error: " << query.lastError().text();
            }
        } else {  // Если запись не существует
            // Подготовка запроса на вставку записи в базу данных
            query.prepare("INSERT INTO database (day, time, subject, homework, checked, row) VALUES (:day, :time, :subject, :homework, :checked, :row)");
            query.bindValue(":day", day);
            query.bindValue(":time", time);
            query.bindValue(":subject", subject);
            query.bindValue(":homework", homework);
            query.bindValue(":checked", 0);
            query.bindValue(":row", row);
            if (!query.exec()) {  // Если запрос не выполнен
                qDebug() << "Failed to insert data into the database";
                qDebug() << "Error: " << query.lastError().text();
            }
        }
    }
}



void MainWindow::onCellClicked(int row, int column)
{
    QTableWidgetItem *item = tableWidget->item(row, column);  // Получение элемента таблицы по указанным строке и столбцу

    if (item) {  // Если элемент существует
        QString data = item->text();  // Получение текста элемента

        if (column == 0) {  // Если кликнутый столбец - это первый столбец (столбец времени)
            SetTime setTime;  // Создание объекта класса SetTime
            setTime.setTime(data);  // Установка времени в объект setTime

            if (setTime.exec() == QDialog::Accepted) {  // Если диалоговое окно setTime было принято
                QTime startTime = setTime.getStartTime();  // Получение начального времени
                QTime endTime = setTime.getEndTime();  // Получение конечного времени

                QString timeString = startTime.toString("hh:mm") + " - " + endTime.toString("hh:mm");  // Формирование строки времени
                QTableWidgetItem *timeItem = new QTableWidgetItem(timeString);  // Создание нового элемента таблицы с этим временем

                tableWidget->setItem(row, column, timeItem);  // Установка нового элемента в таблицу
            }
        }
        else {  // Если кликнутый столбец - это не первый столбец (столбец с днями недели)
            addPlans->setColumn(column);  // Установка номера столбца в объект addPlans
            addPlans->setRow(row);  // Установка номера строки в объект addPlans
            addPlans->show();  // Отображение диалогового окна addPlans

            QTableWidgetItem *headerItem = tableWidget->horizontalHeaderItem(column);  // Получение заголовка столбца
            QString day = headerItem->text();  // Получение текста заголовка (день недели)
            QPair<QString, QString> subjectAndHomework = loadDataFromDatabase(day, QString::number(row));  // Загрузка данных из базы данных
            addPlans->setData(subjectAndHomework.first, subjectAndHomework.second);  // Установка данных в объект addPlans

            if (addPlans->exec() == QDialog::Accepted) {  // Если диалоговое окно addPlans было принято
                QString subject = addPlans->getSubject();  // Получение предмета
                QString homework = addPlans->getHomework();  // Получение домашнего задания

                QTableWidgetItem *timeItem = tableWidget->item(row, 0);  // Получение элемента с временем
                if (timeItem) {  // Если элемент с временем существует
                    QString timeString = timeItem->text();  // Получение текста элемента (времени)
                    insertDataIntoDatabase(day, timeString, subject, homework, "", QString::number(row));  // Вставка данных в базу данных

                    QTableWidgetItem *subjectItem = new QTableWidgetItem(subject);  // Создание нового элемента таблицы с предметом
                    tableWidget->setItem(row, column, subjectItem);  // Установка нового элемента в таблицу
                }
            }
        }
    }
    else {  // Если элемента не существует
        if (column == 0) {  // Если кликнутый столбец - это первый столбец (столбец времени)
            SetTime setTime;  // Создание объекта класса SetTime
            setTime.clear();  // Очистка объекта setTime

            if (setTime.exec() == QDialog::Accepted) {  // Если диалоговое окно setTime было принято
                QTime startTime = setTime.getStartTime();  // Получение начального времени
                QTime endTime = setTime.getEndTime();  // Получение конечного времени

                QString timeString = startTime.toString("hh:mm") + " - " + endTime.toString("hh:mm");  // Формирование строки времени
                QTableWidgetItem *timeItem = new QTableWidgetItem(timeString);  // Создание нового элемента таблицы с этим временем

                tableWidget->setItem(row, column, timeItem);  // Установка нового элемента в таблицу
            }
        }
        else {  // Если кликнутый столбец - это не первый столбец (столбец с днями недели)
            addPlans->clear();  // Очистка объекта addPlans

            if (addPlans->exec() == QDialog::Accepted) {  // Если диалоговое окно addPlans было принято
                QString subject = addPlans->getSubject();  // Получение предмета
                QString homework = addPlans->getHomework();  // Получение домашнего задания

                QTableWidgetItem *subjectItem = new QTableWidgetItem(subject);  // Создание нового элемента таблицы с предметом

                tableWidget->setItem(row, column, subjectItem);  // Установка нового элемента в таблицу

                QTableWidgetItem *headerItem = tableWidget->horizontalHeaderItem(column);  // Получение заголовка столбца
                QString day = headerItem->text();  // Получение текста заголовка (день недели)

                QTableWidgetItem *timeItem = tableWidget->item(row, 0);  // Получение элемента с временем
                if (timeItem) {  // Если элемент с временем существует
                    QString timeString = timeItem->text();  // Получение текста элемента (времени)
                    insertDataIntoDatabase(day, timeString, subject, homework, "", QString::number(row));  // Вставка данных в базу данных
                }
            }
        }
    }
    saveDataToTextFile();  // Сохранение данных в текстовый файл
}



// Слот, который вызывается при клике на заголовок столбца таблицы
void MainWindow::onDayClicked(int column)
{
    if (column != 0) {  // Если кликнутый столбец - это не первый столбец (столбец времени)
        QTableWidgetItem *headerItem = tableWidget->horizontalHeaderItem(column);  // Получение заголовка столбца
        QString day = headerItem->text();  // Получение текста заголовка (день недели)
        DayPlans dayPlans(day, this);  // Создание объекта класса DayPlans
        dayPlans.exec();  // Отображение диалогового окна dayPlans
    }
}

// Слот, который вызывается при изменении времени
void MainWindow::onTimeChanged(const QTime &time)
{
    int row = tableWidget->currentRow();  // Получение номера текущей строки
    int column = tableWidget->currentColumn();  // Получение номера текущего столбца

    QTableWidgetItem *timeItem = new QTableWidgetItem(time.toString("hh:mm"));  // Создание нового элемента таблицы с временем
    tableWidget->setItem(row, column, timeItem);  // Установка нового элемента в таблицу
}

// Метод для загрузки предмета из базы данных
QString MainWindow::loadSubjectFromDatabase(const QString& day, const QString& row)
{
    QSettings settings("YourCompany", "YourApp");  // Создание объекта класса QSettings
    QSqlQuery query;  // Создание объекта класса QSqlQuery

    // Подготовка запроса на выборку из базы данных
    query.prepare("SELECT time, subject, homework, checked FROM database WHERE day = :day AND row = :row");
    query.bindValue(":day", day);
    query.bindValue(":row", row);

    if (!query.exec()) {  // Если запрос не выполнен
        qDebug() << "Failed to execute query";
        qDebug() << "Error: " << query.lastError().text();
        return QString();
    }

    if (query.first()) {  // Если запись существует
        QString time = query.value(0).toString();  // Получение времени
        QString subject = query.value(1).toString();  // Получение предмета
        QString homework = query.value(2).toString();  // Получение домашнего задания
        int checked = query.value(3).toInt();  // Получение значения checked

        return subject;  // Возвращение предмета
    } else {
        return QString();  // Возвращение пустой строки, если запись не существует
    }
}

// Метод для загрузки предмета и домашнего задания из базы данных
QPair<QString, QString> MainWindow::loadDataFromDatabase(const QString& day, const QString& row)
{
    QSqlQuery query;  // Создание объекта QSqlQuery

    // Подготовка запроса на выборку из базы данных
    query.prepare("SELECT subject, homework FROM database WHERE day = :day AND row = :row");
    query.bindValue(":day", day);
    query.bindValue(":row", row);

    if (!query.exec()) {  // Если запрос не выполнен
        qDebug() << "Failed to execute query";
        qDebug() << "Error: " << query.lastError().text();
        return QPair<QString, QString>();
    }

    if (query.first()) {  // Если запись существует
        QString subject = query.value(0).toString();  // Получение предмета
        QString homework = query.value(1).toString();  // Получение домашнего задания
        return qMakePair(subject, homework);  // Возвращение пары предмет-домашнее задание
    } else {
        return QPair<QString, QString>();  // Возвращение пустой пары, если запись не существует
    }
}

// Метод для загрузки времени из базы данных
void MainWindow::loadTimeFromDatabase()
{
    QSqlQuery query;  // Создание объекта QSqlQuery

    // Подготовка запроса на выборку из базы данных
    query.prepare("SELECT DISTINCT time FROM database ORDER BY time");

    if (query.exec()) {  // Если запрос выполнен
        int row = 0;
        while (query.next()) {  // Пока есть записи
            QString time = query.value(0).toString();  // Получение времени
            QTableWidgetItem *timeItem = new QTableWidgetItem(time);  // Создание нового элемента таблицы с этим временем
            tableWidget->setItem(row, 0, timeItem);  // Установка нового элемента в таблицу
            ++row;
        }
    } else {
        qDebug() << "Failed to load data from the database";
        qDebug() << "Error: " << query.lastError().text();
    }
}

// Метод для поиска элемента таблицы
QTableWidgetItem* MainWindow::findTableWidgetItem(const QString& day, const QString& time, const QString& subject, const QString& homework, bool checked, const QString& row)
{
    QSqlQuery query;  // Создание объекта QSqlQuery

    // Подготовка запроса на выборку из базы данных
    query.prepare("SELECT * FROM database WHERE day = :day AND time = :time AND subject = :subject AND homework = :homework AND checked = :checked AND row = :row");
    query.bindValue(":day", day);
    query.bindValue(":time", time);
    query.bindValue(":subject", subject);
    query.bindValue(":homework", homework);
    query.bindValue(":checked", checked ? 1 : 0);
    query.bindValue(":row", row);

    if (query.exec() && query.next()) {  // Если запрос выполнен и запись существует

        int rows = tableWidget->rowCount();  // Получение количества строк таблицы
        int cols = tableWidget->columnCount();  // Получение количества столбцов таблицы
        for(int i = 0; i < rows; ++i) {
            for(int j = 0; j < cols; ++j) {
                QTableWidgetItem *item = tableWidget->item(i, j);  // Получение элемента таблицы
                if (item && item->text() == subject) {  // Если элемент существует и его текст совпадает с предметом
                    return item;  // Возвращение элемента
                }
            }
        }
    }

    return nullptr;  // Возвращение nullptr, если элемент не найден
}

// Метод для проверки, были ли изменены текст или состояние галочки
bool MainWindow::isModified(const QString& day, const QString& time, const QString& subject, const QString& homework)
{
    QSqlQuery query;  // Создание объекта QSqlQuery

    // Подготовка запроса на выборку из базы данных
    query.prepare("SELECT subject, homework, checked FROM database WHERE day = :day AND time = :time");
    query.bindValue(":day", day);
    query.bindValue(":time", time);

    if (query.exec()) {  // Если запрос выполнен
        if (query.first()) {  // Если запись существует
            QString originalSubject = query.value(0).toString();  // Получение оригинального предмета
            QString originalHomework = query.value(1).toString();  // Получение оригинального домашнего задания

            // Если текст или состояние галочки были изменены, верните true
            if (subject != originalSubject || homework != originalHomework) {
                return true;
            }
        }
    } else {
        qDebug() << "Failed to query data from the database";
        qDebug() << "Error: " << query.lastError().text();
    }

    // Если текст и состояние галочки не были изменены, верните false
    return false;
}


// Метод для сохранения данных в текстовый файл
void MainWindow::saveDataToTextFile()
{
    QFile file("schedule.txt");  // Создание объекта QFile
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))  // Открытие файла для записи
        return;

    QTextStream out(&file);  // Создание объекта QTextStream для записи в файл
    int rows = tableWidget->rowCount();  // Получение количества строк таблицы
    int columns = tableWidget->columnCount();  // Получение количества столбцов таблицы
    for(int i = 0; i < rows; ++i) {
        for(int j = 1; j < columns; ++j) {
            QTableWidgetItem *item = tableWidget->item(i, j);  // Получение элемента таблицы
            if(item) {  // Если элемент существует
                QString day = tableWidget->horizontalHeaderItem(j)->text();  // Получение дня недели
                QString time = tableWidget->item(i, 0)->text();  // Получение времени
                QString subject = item->text();  // Получение предмета
                // Получение домашнего задания из базы данных
                QPair<QString, QString> subjectAndHomework = loadDataFromDatabase(day, QString::number(i));
                QString homework = subjectAndHomework.second;
                out << day << ", " << time << ", " << subject << ", " << homework << "\n";  // Запись данных в файл
            }
        }
    }
    file.close();  // Закрытие файла
}

// Метод для обновления заголовка окна
void MainWindow::updateWindowTitle() {
    int elapsedSeconds = startTime.secsTo(QDateTime::currentDateTime());  // Вычисление прошедших секунд
    setWindowTitle(QString("Application running for %1 seconds").arg(elapsedSeconds));  // Установка нового заголовка окна
}

// Метод для удаления записи из базы данных
void MainWindow::deleteRecord(int column, int row)
{
    // Получение дня и времени из заголовка таблицы и ячейки
    QTableWidgetItem *headerItem = tableWidget->horizontalHeaderItem(column);
    QString day = headerItem->text();
    QTableWidgetItem *timeItem = tableWidget->item(row, 0);
    QString timeString = timeItem->text();

    // Подготовка SQL-запроса для удаления записи
    QSqlQuery query;
    query.prepare("DELETE FROM database WHERE day = :day AND time = :time AND row = :row");
    query.bindValue(":day", day);
    query.bindValue(":time", timeString);
    query.bindValue(":row", QString::number(row));

    // Выполнение запроса и обработка возможных ошибок
    if (!query.exec()) {
        qDebug() << "Failed to delete data from the database";
        qDebug() << "Error: " << query.lastError().text();
    }
}

