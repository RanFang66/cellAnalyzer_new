#ifndef USERMANAGEUI_H
#define USERMANAGEUI_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlTableModel>
#include <QItemSelectionModel>
#include <QDataWidgetMapper>

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
private slots:
    void on_btnAddUser_clicked();

    void on_btnDeleteUser_clicked();

    void on_btnEditUser_clicked();

    void onCurrentRowChanged(QModelIndex &curr, QModelIndex &prev);

private:
    Ui::UserManageUi *ui;

    enum USER_EDIT_STATE {
        USER_INFO_IDLE = 0,
        USER_INFO_ADDING,
        USER_INFO_EDITING
    };

    QSqlDatabase *userDb;
    QSqlTableModel  *tblModel;
    QItemSelectionModel *theSelection;
    QDataWidgetMapper *dataMapper;
    int editState;
    QString currName;
    QString passwd;
    QString passwdConfirm;
    QString email;
    QString department;
    QString phone;
    int curID;
    int authority;
};

#endif // USERMANAGEUI_H
