#include "settime.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QMessageBox>
#include <QLabel>

// Конструктор класса SetTime
SetTime::SetTime(QWidget *parent)
    : QDialog(parent)
{
    this->setWindowTitle("Время урока");  // Установка заголовка окна
    QLabel *startTimeLabel = new QLabel("Введите время начала урока", this);  // Создание метки для ввода времени начала урока
    startTimeEdit = new QTimeEdit(this);  // Создание виджета для ввода времени начала урока
    QLabel *endTimeLabel = new QLabel("Введите время конца урока", this);  // Создание метки для ввода времени конца урока
    endTimeEdit = new QTimeEdit(this);  // Создание виджета для ввода времени конца урока
    okButton = new QPushButton("OK", this);  // Создание кнопки "ОК"
    cancelButton = new QPushButton("Отмена", this);  // Создание кнопки "Отмена"

    QHBoxLayout *buttonLayout = new QHBoxLayout;  // Создание горизонтального компоновщика для кнопок
    buttonLayout->addWidget(okButton);  // Добавление кнопки "ОК" в компоновщик
    buttonLayout->addWidget(cancelButton);  // Добавление кнопки "Отмена" в компоновщик

    QVBoxLayout *mainLayout = new QVBoxLayout;  // Создание вертикального компоновщика для всего окна
    mainLayout->addWidget(startTimeLabel);  // Добавление метки для ввода времени начала урока в компоновщик
    mainLayout->addWidget(startTimeEdit);  // Добавление виджета для ввода времени начала урока в компоновщик
    mainLayout->addWidget(endTimeLabel);  // Добавление метки для ввода времени конца урока в компоновщик
    mainLayout->addWidget(endTimeEdit);  // Добавление виджета для ввода времени конца урока в компоновщик
    mainLayout->addLayout(buttonLayout);  // Добавление компоновщика кнопок в основной компоновщик

    setLayout(mainLayout);  // Установка основного компоновщика в качестве компоновщика для окна

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));  // Подключение сигнала нажатия кнопки "ОК" к слоту accept()
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));  // Подключение сигнала нажатия кнопки "Отмена" к слоту reject()

    QSettings settings("MyApp", "TimeSettings");  // Создание объекта QSettings
    startTimeEdit->setTime(settings.value("startTime").toTime());  // Установка времени начала урока из настроек
    endTimeEdit->setTime(settings.value("endTime").toTime());  // Установка времени конца урока из настроек
}

// Метод для получения времени начала урока
QTime SetTime::getStartTime() const
{
    return startTimeEdit->time();  // Возвращение времени из виджета startTimeEdit
}

// Метод для получения времени конца урока
QTime SetTime::getEndTime() const
{
    return endTimeEdit->time();  // Возвращение времени из виджета endTimeEdit
}

// Метод для установки времени в виджеты
void SetTime::setTime(const QString &time)
{
    QStringList parts = time.split("-");  // Разделение строки времени на части
    if (parts.size() == 2) {  // Если строка времени содержит две части (время начала и конца урока)
        QTime startTime = QTime::fromString(parts[0].trimmed(), "hh:mm");  // Преобразование строки в QTime
        QTime endTime = QTime::fromString(parts[1].trimmed(), "hh:mm");  // Преобразование строки в QTime
        startTimeEdit->setTime(startTime);  // Установка времени начала урока
        endTimeEdit->setTime(endTime);  // Установка времени конца урока
    }
}

// Метод для очистки виджетов от времени
void SetTime::clear()
{
    startTimeEdit->setTime(QTime(0, 0));  // Установка времени начала урока на 0
    endTimeEdit->setTime(QTime(0, 0));  // Установка времени конца урока на 0
}

// Метод для обработки нажатия кнопки "ОК"
void SetTime::accept()
{
    if (endTimeEdit->time() < startTimeEdit->time()) {  // Если время конца урока меньше времени начала урока
        QMessageBox::warning(this, "Ошибка", "Время окончания не может быть меньше времени начала. Пожалуйста, введите данные снова.");  // Вывод предупреждения об ошибке
        return;
    }
    QSettings settings("MyApp", "TimeSettings");  // Создание объекта QSettings
    settings.setValue("startTime", startTimeEdit->time());  // Сохранение времени начала урока в настройках
    settings.setValue("endTime", endTimeEdit->time());  // Сохранение времени конца урока в настройках

    QDialog::accept();  // Вызов метода accept() базового класса
}
