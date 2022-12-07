#include "UserInfoEditDialog.h"
#include "ui_UserInfoEditDialog.h"
#include <QCryptographicHash>
#include <QMessageBox>

UserInfoEditDialog::UserInfoEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserInfoEditDialog)
{
    ui->setupUi(this);
    ui->cBoxAuthority->clear();
    ui->cBoxAuthority->addItem("Admin");
    ui->cBoxAuthority->addItem("Senior User");
    ui->cBoxAuthority->addItem("Normal User");
    ui->cBoxAuthority->addItem("Guest");
}

UserInfoEditDialog::~UserInfoEditDialog()
{
    delete ui;
}

void UserInfoEditDialog::setUpdateRecord(QSqlRecord &recData)
{
    mRecord = recData;

    setWindowTitle("Edit User Info");

    ui->editName->setText(recData.value("name").toString());
    ui->editPasswd->setText(recData.value("password").toString());
    ui->editEmail->setText(recData.value("email").toString());
    ui->editDepart->setText(recData.value("department").toString());
    ui->editPhone->setText(recData.value("phone").toString());
    ui->cBoxAuthority->setCurrentIndex(recData.value("authority").toInt());
}

void UserInfoEditDialog::setInsertRecord(QSqlRecord &recData)
{
    mRecord = recData;

    setWindowTitle("Add New User");
}

QSqlRecord UserInfoEditDialog::getRecordData()
{
    mRecord.setValue("name", ui->editName->text());
    mRecord.setValue("password", encryptPasswd(ui->editPasswd->text()));
    mRecord.setValue("email", ui->editEmail->text());
    mRecord.setValue("phone", ui->editPhone->text());
    mRecord.setValue("department", ui->editDepart->text());
    mRecord.setValue("authority", ui->cBoxAuthority->currentIndex());
    return mRecord;
}

void UserInfoEditDialog::on_btnConfirm_clicked()
{
    QString pswd = ui->editPasswd->text();
    QString pswdAgain = ui->editConfirm->text();

    if (pswd == pswdAgain) {
        this->accept();
    } else {
        QMessageBox::critical(this, "input error", "password confirm failed");
        return;
    }
}

QString UserInfoEditDialog::encryptPasswd(const QString &str)
{
    QByteArray btArray;
    btArray.append(str.toLatin1());
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(btArray);
    QByteArray resultArray = hash.result();
    QString md5 = resultArray.toHex();
    return md5;
}
