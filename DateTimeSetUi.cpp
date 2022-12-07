#include "DateTimeSetUi.h"
#include "ui_DateTimeSetUi.h"

DateTimeSetUi::DateTimeSetUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateTimeSetUi)
{
    ui->setupUi(this);
}

DateTimeSetUi::~DateTimeSetUi()
{
    delete ui;
}
