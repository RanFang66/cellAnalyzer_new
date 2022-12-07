#include "UserManageUi.h"
#include "ui_UserManageUi.h"
#include <QMessageBox>
#include <QDebug>
UserManageUi::UserManageUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserManageUi)
{
    ui->setupUi(this);

    ui->tblUserInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblUserInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblUserInfo->setAlternatingRowColors(true);

    userDb = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(userDb);
    initUserInfoTable();
}

UserManageUi::~UserManageUi()
{
    delete ui;
}

void UserManageUi::initUserInfoTable()
{
    qryModel = new QSqlQueryModel(this);
    theSelection = new QItemSelectionModel(qryModel);
    qryModel->setQuery("SELECT userId, name, authority, email, phone, department FROM userInfo ORDER BY userId", userDb);
    if (qryModel->lastError().isValid()) {
        qDebug() << "Error: query user information failed!";
        return;
    }

    qryModel->setHeaderData(0, Qt::Horizontal, "user ID");
    qryModel->setHeaderData(1, Qt::Horizontal, "user Name");
    qryModel->setHeaderData(2, Qt::Horizontal, "authority");
    qryModel->setHeaderData(3, Qt::Horizontal, "e-mail");
    qryModel->setHeaderData(4, Qt::Horizontal, "phone");
    qryModel->setHeaderData(5, Qt::Horizontal, "department");

    ui->tblUserInfo->setModel(qryModel);
    ui->tblUserInfo->setSelectionModel(theSelection);
    ui->tblUserInfo->resizeColumnsToContents();

    ui->btnAddUser->setEnabled(true);
    ui->btnDeleteUser->setEnabled(true);
    ui->btnEditUser->setEnabled(true);
}

void UserManageUi::on_btnAddUser_clicked()
{
    insertRecord();
    qryModel->query().exec();
}

void UserManageUi::on_btnDeleteUser_clicked()
{
    int curRecNo = theSelection->currentIndex().row();
    QSqlRecord curRec = qryModel->record(curRecNo);
    if (curRec.isEmpty())
        return;

    int id = curRec.value("userId").toInt();
    query->prepare("DELETE FORM userInfo WHERE userId = :ID");
    query->bindValue(":ID", id);

    QMessageBox::StandardButton btn = QMessageBox::question(this, "Confirm Deletion", "Delete this user?");
    if (btn == QMessageBox::Ok) {
        if (!query->exec()) {
            QMessageBox::critical(this, "Error", "Failed to delete the user"
            + query->lastError().text());
        } else {
            qryModel->query().exec();
        }
    }
}

void UserManageUi::on_btnEditUser_clicked()
{
    int curRecNo = theSelection->currentIndex().row();
    updateRecord(curRecNo);
    qryModel->query().exec();
}

void UserManageUi::onCurrentRowChanged(QModelIndex &curr, QModelIndex &prev)
{

}

void UserManageUi::updateRecord(int recNo)
{
    QSqlRecord curRec = qryModel->record(recNo);
    int id = curRec.value("userId").toInt();



    query->prepare("SELECT * FROM userInfo WHERE userId = :ID");
    query->bindValue(":ID", id);
    query->exec();
    query->first();

    if (!query->isValid())
        return;
    curRec = query->record();

    UserInfoEditDialog *infoDialog = new UserInfoEditDialog(this);
    Qt::WindowFlags flags = infoDialog->windowFlags();
    infoDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);

    infoDialog->setUpdateRecord(curRec);
    int ret = infoDialog->exec();
    if (ret == QDialog::Accepted) {
        QSqlRecord recData = infoDialog->getRecordData();

        query->prepare("UPDATE userInfo SET name=:name, authority=:authority, password=:password, "
                        "email=:email, phone=:phone, department=:department WHERE userId = :id");
        query->bindValue(":name", recData.value("name"));
        query->bindValue(":authority", recData.value("authority"));
        query->bindValue(":password", recData.value("password"));
        query->bindValue(":email", recData.value("email"));
        query->bindValue(":phone", recData.value("phone"));
        query->bindValue(":department", recData.value("department"));
        query->bindValue(":ID", id);
        if (!query->exec()) {
            QMessageBox::critical(this, "Error", "Failed to update the user information"
            + query->lastError().text());
        } else {
            qryModel->query().exec();
        }
    }

    delete infoDialog;
}

void UserManageUi::insertRecord()
{
    query->exec("SELECT * FROM userInfo WHERE userId = -1");
    QSqlRecord  curRec = query->record();
    curRec.setValue("userId", qryModel->rowCount()+1234);

    UserInfoEditDialog *infoDialog = new UserInfoEditDialog(this);
    Qt::WindowFlags flags = infoDialog->windowFlags();
    infoDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);

    infoDialog->setInsertRecord(curRec);
    int ret = infoDialog->exec();
    if (ret == QDialog::Accepted) {
        QSqlRecord recData = infoDialog->getRecordData();
        query->prepare("INSERT INTO userInfo (userId, name, password, authority, email, phone, department) "
            "VALUES(:ID, :name, :password, :authority, :email, :phone, :department)");
        query->bindValue(":ID", recData.value("userId"));
        query->bindValue(":name", recData.value("name"));
        query->bindValue(":authority", recData.value("authority"));
        query->bindValue(":password", recData.value("password"));
        query->bindValue(":email", recData.value("email"));
        query->bindValue(":phone", recData.value("phone"));
        query->bindValue(":department", recData.value("department"));

        if (!query->exec()) {
            QMessageBox::critical(this, "Error", "Failed to insert the user information"
            + query->lastError().text());
        } else {
            qryModel->query().exec();
        }
    }
    delete infoDialog;
}

void UserManageUi::on_btnReturn_clicked()
{
    emit return2sysSetting();
}
