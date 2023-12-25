#include "dayplans.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QFile>
#include <QSettings>
#include <QFileDialog>

// Конструктор класса DayPlans
DayPlans::DayPlans(const QString &day, MainWindow *mainWindow, QWidget *parent)
: QDialog(parent), day(day), mainWindow(mainWindow)
{
    this->setWindowTitle("Домашнее задание на день");  // Установка заголовка окна
    tableWidget = new QTableWidget(4, 8, this);  // Создание виджета таблицы
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Отключение возможности редактирования ячеек таблицы
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // Установка режима растягивания столбцов таблицы
    QStringList headers;  // Создание списка заголовков
    headers << "Время" << "Предмет" << "Домашнее задание" << "Выполнено";  // Добавление заголовков в список
    tableWidget->setHorizontalHeaderLabels(headers);  // Установка заголовков таблицы
    tableWidget->horizontalHeader()->setVisible(true);  // Включение отображения горизонтальных заголовков
    tableWidget->update();  // Обновление таблицы

    QPushButton *okButton = new QPushButton("OK", this);  // Создание кнопки "ОК"
    QPushButton *cancelButton = new QPushButton("Отмена", this);  // Создание кнопки "Отмена"
    saveToFileButton = new QPushButton("Сохранить", this);  // Создание кнопки "Сохранить"
    connect(saveToFileButton, SIGNAL(clicked()), this, SLOT(saveTableToFile()));  // Подключение сигнала нажатия кнопки "Сохранить" к слоту saveTableToFile()

    connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));  // Подключение сигнала нажатия кнопки "ОК" к слоту okButtonClicked()
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));  // Подключение сигнала нажатия кнопки "Отмена" к слоту reject()

    QHBoxLayout *buttonLayout = new QHBoxLayout;  // Создание горизонтального компоновщика для кнопок
    buttonLayout->addWidget(okButton);  // Добавление кнопки "ОК" в компоновщик
    buttonLayout->addWidget(cancelButton);  // Добавление кнопки "Отмена" в компоновщик
    buttonLayout->addWidget(saveToFileButton);  // Добавление кнопки "Сохранить" в компоновщик

    QVBoxLayout *mainLayout = new QVBoxLayout;  // Создание вертикального компоновщика для всего окна
    mainLayout->addWidget(tableWidget);  // Добавление таблицы в компоновщик
    mainLayout->addLayout(buttonLayout);  // Добавление компоновщика кнопок в основной компоновщик

    setLayout(mainLayout);  // Установка основного компоновщика в качестве компоновщика для окна

    resize(800, 600);  // Изменение размера окна

    QSqlQuery query;  // Создание объекта класса QSqlQuery
    query.prepare("SELECT time, subject, homework, checked FROM database WHERE day = :day");  // Подготовка запроса на выборку из базы данных
    query.bindValue(":day", day);  // Привязка значения дня к запросу
    if (query.exec()) {  // Если запрос выполнен успешно
        int row = 0;  // Номер строки
        while (query.next()) {  // Пока есть записи
            QString time = query.value(0).toString();  // Получение времени
            QString subject = query.value(1).toString();  // Получение предмета
            QString homework = query.value(2).toString();  // Получение домашнего задания
            int checked = query.value(3).toInt();  // Получение значения checked

            tableWidget->setRowCount(row+1);  // Установка количества строк таблицы
            tableWidget->setColumnCount(4);  // Установка количества столбцов таблицы
            tableWidget->setItem(row, 0, new QTableWidgetItem(time));  // Установка времени в ячейку таблицы
            tableWidget->setItem(row, 1, new QTableWidgetItem(subject));  // Установка предмета в ячейку таблицы
            tableWidget->setItem(row, 2, new QTableWidgetItem(homework));  // Установка домашнего задания в ячейку таблицы

            QTableWidgetItem *checkItem = new QTableWidgetItem();  // Создание нового элемента таблицы
            checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);  // Установка флагов для элемента
            checkItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);  // Установка состояния флажка
            tableWidget->setItem(row, 3, checkItem);  // Установка элемента в ячейку таблицы
            row++;  // Увеличение номера строки на 1
        }
    } else {  // Если запрос не выполнен
        qDebug() << "Failed to query data from the database";  // Вывод сообщения об ошибке
        qDebug() << "Error: " << query.lastError().text();  // Вывод текста ошибки
    }
}

// Деструктор класса DayPlans
DayPlans::~DayPlans()
{
    delete tableWidget;  // Удаление виджета таблицы
}


// Метод, вызываемый при нажатии кнопки "ОК"
void DayPlans::okButtonClicked()
{
    QSettings settings("YourCompany", "YourApp");  // Создание объекта QSettings
    int rows = tableWidget->rowCount();  // Получение количества строк таблицы
    for(int i = 0; i < rows; ++i) {  // Цикл по строкам таблицы
        QTableWidgetItem *item = tableWidget->item(i, 3); // 3 - это номер столбца с флажками
        if (item) {  // Если элемент существует
            int checked = (item->checkState() == Qt::Checked);  // Получение состояния флажка
            QString time = tableWidget->item(i, 0)->text();  // Получение времени
            QString subject = tableWidget->item(i, 1)->text();  // Получение предмета
            QString homework = tableWidget->item(i, 2)->text();  // Получение домашнего задания

            if (mainWindow->isModified(day, time, subject, homework)) {  // Если данные были изменены
                checked = 0;  // Сброс состояния флажка
                item->setCheckState(Qt::Unchecked);  // Установка состояния флажка в "не отмечено"
            }

            mainWindow->insertDataIntoDatabase(day, time, subject, homework, checked, QString::number(i));  // Вставка данных в базу данных

            settings.setValue(day + "/" + time + "/highlighted", checked);  // Сохранение значения в настройках
            QTableWidgetItem *mainFormItem = mainWindow->findTableWidgetItem(day, time, subject, homework, checked, QString::number(i));  // Поиск элемента в главной форме
            if (mainFormItem) {  // Если элемент найден
                if (checked) {  // Если флажок установлен
                    mainFormItem->setBackground(Qt::green);  // Окрашивание элемента в зеленый цвет
                } else {  // Если флажок не установлен
                    mainFormItem->setBackground(Qt::white); // Снятие окрашивания
                }
            }
        }
    }
    accept();  // Принятие диалогового окна
}

// Метод для сохранения таблицы в файл
void DayPlans::saveTableToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "Текстовые файлы (*.txt)"); // Вызов диалогового окна для выбора имени файла и места для сохранения

    if (!fileName.isEmpty()) { // Проверка, выбран ли файл
        QFile file(fileName);  // Создание объекта QFile
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {  // Открытие файла для записи
            QTextStream out(&file);  // Создание объекта QTextStream для записи в файл
            int rows = tableWidget->rowCount();  // Получение количества строк таблицы
            int cols = tableWidget->columnCount();  // Получение количества столбцов таблицы
            for (int i = 0; i < rows; ++i) {  // Цикл по строкам таблицы
                for (int j = 0; j < cols; ++j) {  // Цикл по столбцам таблицы
                    QTableWidgetItem *item = tableWidget->item(i, j);  // Получение элемента таблицы
                    if (item) {  // Если элемент существует
                        out << item->text() << "\t";  // Запись текста элемента в файл
                    }
                }
                out << "\n";  // Запись символа новой строки в файл
            }
            file.close();  // Закрытие файла
        }
    }
}


