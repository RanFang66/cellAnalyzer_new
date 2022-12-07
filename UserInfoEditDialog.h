#ifndef USERINFOEDITDIALOG_H
#define USERINFOEDITDIALOG_H

#include <QDialog>
#include <QSqlRecord>
namespace Ui {
class UserInfoEditDialog;
}

class UserInfoEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserInfoEditDialog(QWidget *parent = nullptr);
    ~UserInfoEditDialog();

    void setUpdateRecord(QSqlRecord &recData);
    void setInsertRecord(QSqlRecord &recData);
    QSqlRecord getRecordData();


private slots:
    void on_btnConfirm_clicked();

private:
    Ui::UserInfoEditDialog *ui;
    QSqlRecord mRecord;

    QString encryptPasswd(const QString &str);
};

#endif // USERINFOEDITDIALOG_H
