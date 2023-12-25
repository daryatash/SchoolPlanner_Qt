#include "addplans.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QLabel>
#include <QDialogButtonBox>
#include <QSqlQuery>
#include <QDebug>
#include <QTableWidget>
#include <QSqlError>

// Конструктор класса AddPlans
AddPlans::AddPlans(QWidget *parent)
    : QDialog(parent), isModified(false)
{
    this->setWindowTitle("Домашнее задание");  // Установка заголовка окна

    QLabel *subjectLabel = new QLabel("Введите название предмета", this);  // Создание метки для ввода названия предмета
    subjectLineEdit = new QLineEdit(this);  // Создание виджета для ввода названия предмета

    QLabel *homeworkLabel = new QLabel("Введите домашнее задание", this);  // Создание метки для ввода домашнего задания
    homeworkTextEdit = new QTextEdit(this);  // Создание виджета для ввода домашнего задания

    QLabel *buttonLabel = new QLabel("Для просмотра списка домашних заданий на день нажмите на название дня недели в таблице", this);  // Создание метки с инструкцией
    okButton = new QPushButton("OK", this);  // Создание кнопки "ОК"
    cancelButton = new QPushButton("Отмена", this);  // Создание кнопки "Отмена"
    setNotificationButton = new QPushButton("Напомнить", this);  // Создание кнопки "Напомнить"
    deleteButton = new QPushButton("Удалить", this);  // Создание кнопки "Удалить"

    QHBoxLayout *buttonLayout = new QHBoxLayout;  // Создание горизонтального компоновщика для кнопок
    buttonLayout->addWidget(okButton);  // Добавление кнопки "ОК" в компоновщик
    buttonLayout->addWidget(cancelButton);  // Добавление кнопки "Отмена" в компоновщик
    buttonLayout->addWidget(setNotificationButton);  // Добавление кнопки "Напомнить" в компоновщик
    buttonLayout->addWidget(deleteButton);  // Добавление кнопки "Удалить" в компоновщик

    QVBoxLayout *mainLayout = new QVBoxLayout;  // Создание вертикального компоновщика для всего окна
    mainLayout->addWidget(subjectLabel);  // Добавление метки для ввода названия предмета в компоновщик
    mainLayout->addWidget(subjectLineEdit);  // Добавление виджета для ввода названия предмета в компоновщик
    mainLayout->addWidget(homeworkLabel);  // Добавление метки для ввода домашнего задания в компоновщик
    mainLayout->addWidget(homeworkTextEdit);  // Добавление виджета для ввода домашнего задания в компоновщик
    mainLayout->addWidget(buttonLabel);  // Добавление метки с инструкцией в компоновщик
    mainLayout->addLayout(buttonLayout);  // Добавление компоновщика кнопок в основной компоновщик

    setLayout(mainLayout);  // Установка основного компоновщика в качестве компоновщика для окна

    connect(okButton, SIGNAL(clicked()), this, SLOT(saveAndAccept()));  // Подключение сигнала нажатия кнопки "ОК" к слоту saveAndAccept()
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));  // Подключение сигнала нажатия кнопки "Отмена" к слоту reject()
    connect(subjectLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkModified()));  // Подключение сигнала изменения текста в виджете subjectLineEdit к слоту checkModified()
    connect(homeworkTextEdit, SIGNAL(textChanged()), this, SLOT(checkModified()));  // Подключение сигнала изменения текста в виджете homeworkTextEdit к слоту checkModified()
    connect(setNotificationButton, SIGNAL(clicked()), this, SLOT(setNotification()));  // Подключение сигнала нажатия кнопки "Напомнить" к слоту setNotification()
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteAndClear()));  // Подключение сигнала нажатия кнопки "Удалить" к слоту deleteAndClear()

    notificationTimer = new QTimer(this);  // Создание объекта класса QTimer
    connect(notificationTimer, SIGNAL(timeout()), this, SLOT(checkNotification()));  // Подключение сигнала таймера к слоту checkNotification()
    notificationTime = QDateTime::currentDateTime().addYears(100);  // Установка времени уведомления на 100 лет вперед
}

// Метод для получения названия предмета
QString AddPlans::getSubject() const
{
    return subjectLineEdit->text();  // Возвращение текста из виджета subjectLineEdit
}

// Метод для получения домашнего задания
QString AddPlans::getHomework() const
{
    return homeworkTextEdit->toPlainText();  // Возвращение текста из виджета homeworkTextEdit
}

// Слот для сохранения данных и закрытия окна
void AddPlans::saveAndAccept()
{
    if (isModified) {  // Если данные были изменены
        emit colorChanged(Qt::white);  // Изменение цвета на белый
        isModified = false;  // Сброс флага изменения данных
    }

    QDialog::accept();  // Вызов метода accept() базового класса
}

// Метод для установки данных в виджеты
void AddPlans::setData(const QString &subject, const QString &homework)
{
    subjectLineEdit->blockSignals(true);  // Блокировка сигналов виджета subjectLineEdit
    homeworkTextEdit->blockSignals(true);  // Блокировка сигналов виджета homeworkTextEdit

    subjectLineEdit->setText(subject);  // Установка названия предмета в виджет subjectLineEdit
    homeworkTextEdit->setText(homework);  // Установка домашнего задания в виджет homeworkTextEdit

    subjectLineEdit->blockSignals(false);  // Разблокировка сигналов виджета subjectLineEdit
    homeworkTextEdit->blockSignals(false);  // Разблокировка сигналов виджета homeworkTextEdit

    originalSubject = subject;  // Сохранение оригинального названия предмета
    originalHomework = homework;  // Сохранение оригинального домашнего задания

    isModified = false;  // Сброс флага изменения данных
}

// Метод для очистки виджетов
void AddPlans::clear()
{
    subjectLineEdit->clear();  // Очистка виджета subjectLineEdit
    homeworkTextEdit->clear();  // Очистка виджета homeworkTextEdit
}

// Слот для проверки изменения данных
void AddPlans::checkModified()
{
    if (subjectLineEdit->text() != originalSubject || homeworkTextEdit->toPlainText() != originalHomework) {  // Если данные были изменены
        isModified = true;  // Установка флага изменения данных
    }
}


// Метод для установки напоминания
void AddPlans::setNotification()
{
    QDialog *dialog = new QDialog(this);  // Создание нового диалогового окна
    dialog->setWindowTitle("Напоминание");  // Установка заголовка окна

    QVBoxLayout *layout = new QVBoxLayout(dialog);  // Создание вертикального компоновщика

    QLabel *label = new QLabel("Установите дату и время напоминания", dialog);  // Создание метки
    layout->addWidget(label);  // Добавление метки в компоновщик

    QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), dialog);  // Создание виджета для ввода даты и времени
    layout->addWidget(dateTimeEdit);  // Добавление виджета в компоновщик

    QPushButton *okButton = new QPushButton("OK", dialog);  // Создание кнопки "ОК"
    QPushButton *cancelButton = new QPushButton("Отмена", dialog);  // Создание кнопки "Отмена"

    layout->addWidget(okButton);  // Добавление кнопки "ОК" в компоновщик
    layout->addWidget(cancelButton);  // Добавление кнопки "Отмена" в компоновщик

    connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));  // Подключение сигнала нажатия кнопки "ОК" к слоту accept() диалогового окна
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));  // Подключение сигнала нажатия кнопки "Отмена" к слоту reject() диалогового окна

    if (dialog->exec() == QDialog::Accepted) {  // Если диалоговое окно было принято
        Notification notification;  // Создание объекта класса Notification
        notification.time = dateTimeEdit->dateTime();  // Установка времени напоминания
        notification.subject = getSubject();  // Установка предмета
        notification.homework = getHomework();  // Установка домашнего задания
        notifications.append(notification);  // Добавление напоминания в список
        if (!notificationTimer->isActive()) {  // Если таймер не активен
            notificationTimer->start(1000);  // Запуск таймера
        }
    }

    delete dialog;  // Удаление диалогового окна
}

// Метод для проверки напоминаний
void AddPlans::checkNotification()
{
    QDateTime currentTime = QDateTime::currentDateTime();  // Получение текущего времени
    for (int i = 0; i < notifications.size(); ++i) {  // Цикл по всем напоминаниям
        if (currentTime >= notifications[i].time) {  // Если текущее время больше или равно времени напоминания
            QMessageBox::information(this, "Напоминание о домашнем задании", "Предмет: " + notifications[i].subject + "\nДомашнее задание: " + notifications[i].homework);  // Вывод информационного сообщения
            notifications.removeAt(i);  // Удаление напоминания из списка
            --i;  // Уменьшение счетчика на 1
        }
    }
    if (notifications.isEmpty()) {  // Если список напоминаний пуст
        notificationTimer->stop(); // Остановка таймера
    }
}

// Метод для удаления записи и очистки полей ввода
void AddPlans::deleteAndClear()
{
    emit deleteRecordRequested(column, row);  // Вызов сигнала для удаления записи
    subjectLineEdit->clear();  // Очистка поля ввода предмета
    homeworkTextEdit->clear();  // Очистка поля ввода домашнего задания
}


