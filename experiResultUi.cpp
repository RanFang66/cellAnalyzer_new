#include "experiResultUi.h"
#include "ui_experiResultUi.h"

experiResultUi::experiResultUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::experiResultUi)
{
    ui->setupUi(this);
}

experiResultUi::~experiResultUi()
{
    delete ui;
}
