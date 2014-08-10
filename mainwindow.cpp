#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    connectToBD();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow:: connectToBD()
{
    // Подключить файл настроек
    QSettings settings("config.ini", QSettings::IniFormat);

    // Создать объект класса БД
    db = QSqlDatabase::addDatabase("QMYSQL", "base");
    // Записать в файл ошибки в работе (если такие есть)
    if (db.lastError().isValid())
        QMessageBox::critical(this, "Ошибка", db.lastError().text(), QMessageBox::Ok);


    // Считать адрес сервера БД из файла настроек
    db.setHostName(settings.value("database/hostname").toString());
    if (db.lastError().isValid())
        QMessageBox::critical(this, "Ошибка", db.lastError().text(), QMessageBox::Ok);

    // Считать порт для подключения из файла настроек
    db.setPort(settings.value("database/port").toInt());
    if (db.lastError().isValid())
        QMessageBox::critical(this, "Ошибка", db.lastError().text(), QMessageBox::Ok);

    // Считать имя БД на сервере из файла настроек
    db.setDatabaseName("synonyms");
    if (db.lastError().isValid())
        QMessageBox::critical(this, "Ошибка", db.lastError().text(), QMessageBox::Ok);

    // Считать имя пользователя БД из файла настроек
    db.setUserName(settings.value("database/login").toString());
    if (db.lastError().isValid())
        QMessageBox::critical(this, "Ошибка", db.lastError().text(), QMessageBox::Ok);

    // Считать пароль пользователя из файла настроек
    db.setPassword(settings.value("database/password").toString());
    if (db.lastError().isValid())
        QMessageBox::critical(this, "Ошибка", db.lastError().text(), QMessageBox::Ok);


    // Подключиться к БД с заданными параметрами
    bool res = db.open();
    if (db.lastError().isValid())
        QMessageBox::critical(this, "Ошибка", db.lastError().text(), QMessageBox::Ok);


    return res;
}

void MainWindow::on_searchButton_clicked()
{
    int currentId = -1;

    //Запрос на наличие записи
    QSqlQuery *q = new QSqlQuery(db);
    q->prepare("SELECT wtable.id_word FROM wtable WHERE word=?");
    q->bindValue(0, ui->wordLine->text());
    q->exec();

    if (q->next())
    {
        // Идентификатор введенного слова
        currentId = q->value(0).toInt();

        // Сбрасываем запрос
        q->clear();
        QString str = "";

        // Выполняем новый запрос, который вернет нам синонимы
        q->prepare("SELECT wtable.word FROM wtable, stable WHERE stable.id_syn = wtable.id_word AND stable.id_word = ?");
        q->bindValue(0, currentId);
        q->exec();

        if (q->next())
        {
            do
            {
                str.append("-- ").append(q->value(0).toString()).append("\n");
            }
            while (q->next());
        }
        else
        {
            q->clear();
            q->prepare("SELECT wtable.word FROM wtable, stable WHERE stable.id_word = wtable.id_word AND stable.id_syn = ?");
            q->bindValue(0, currentId);
            q->exec();

            while (q->next())
                str.append("-- ").append(q->value(0).toString()).append("\n");
        }

        // Закинуть текст в виджет
        ui->synList->setText(str);
    }
    else
        QMessageBox::critical(0, "Ошибка", "Данного слова нет в базе");
}
