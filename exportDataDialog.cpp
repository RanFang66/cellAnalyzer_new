#include "exportDataDialog.h"
#include "ui_exportDataDialog.h"

exportDataDialog::exportDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exportDataDialog)
{
    ui->setupUi(this);
}

exportDataDialog::~exportDataDialog()
{
    delete ui;
}
