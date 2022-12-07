#ifndef USERMANAGEUI_H
#define USERMANAGEUI_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QItemSelectionModel>
#include <QSqlRecord>
#include <QVariant>

#include "UserInfoEditDialog.h"

namespace Ui {
class UserManageUi;
}

class UserManageUi : public QWidget
{
    Q_OBJECT

public:
    explicit UserManageUi(QWidget *parent = nullptr);
    ~UserManageUi();
    void initUserInfoTable(void);

signals:
    void return2sysSetting();

private slots:
    void on_btnAddUser_clicked();

    void on_btnDeleteUser_clicked();

    void on_btnEditUser_clicked();

    void onCurrentRowChanged(QModelIndex &curr, QModelIndex &prev);

    void on_btnReturn_clicked();

private:
    Ui::UserManageUi *ui;

    enum USER_EDIT_STATE {
        USER_INFO_IDLE = 0,
        USER_INFO_ADDING,
        USER_INFO_EDITING
    };

    QSqlDatabase userDb;
    QSqlQueryModel  *qryModel;
    QItemSelectionModel *theSelection;
    QSqlQuery *query;
//    int editState;
//    QString currName;
//    QString passwd;
//    QString passwdConfirm;
//    QString email;
//    QString department;
//    QString phone;
//    int curID;
//    int authority;

    void updateRecord(int recNo);
    void insertRecord();
};

#endif // USERMANAGEUI_H
