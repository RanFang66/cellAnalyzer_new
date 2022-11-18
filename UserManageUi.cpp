#include "UserManageUi.h"
#include "ui_UserManageUi.h"
#include <QMessageBox>
UserManageUi::UserManageUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserManageUi)
{
    ui->setupUi(this);
}

UserManageUi::~UserManageUi()
{
    delete ui;
}

void UserManageUi::initUserInfoTable()
{
    tblModel = new QSqlTableModel(this, *userDb);
    tblModel->setTable("userInfo");
    tblModel->setSort(tblModel->fieldIndex("userId"), Qt::AscendingOrder);
    tblModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    if (!(tblModel->select())) {
        QMessageBox::critical(this, "Error", "Open data table failed, error info: \n" + tblModel->lastError().text(), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    tblModel->setHeaderData(tblModel->fieldIndex("userId"), Qt::Horizontal, "User Id");
    tblModel->setHeaderData(tblModel->fieldIndex("name"), Qt::Horizontal, "User Name");
    tblModel->setHeaderData(tblModel->fieldIndex("authority"), Qt::Horizontal, "Authority");
    tblModel->setHeaderData(tblModel->fieldIndex("department"), Qt::Horizontal, "Department");
    tblModel->setHeaderData(tblModel->fieldIndex("email"), Qt::Horizontal, "Email");
    tblModel->setHeaderData(tblModel->fieldIndex("phone"), Qt::Horizontal, "Phone");

    theSelection = new QItemSelectionModel(tblModel);
    ui->tblUserInfo->setModel(tblModel);
    ui->tblUserInfo->setSelectionModel(theSelection);
    ui->tblUserInfo->setColumnHidden(tblModel->fieldIndex("password"), true);
}

void UserManageUi::on_btnAddUser_clicked()
{

}

void UserManageUi::on_btnDeleteUser_clicked()
{

}

void UserManageUi::on_btnEditUser_clicked()
{

}
