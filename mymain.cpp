#include "mymain.h"
MyMain::MyMain(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MyMain)
{
    // Инициализировать ui
    ui->setupUi(this);

    // Соединение с БД
    connectToBd();

}

MyMain::~MyMain()
{
    delete ui;
}

