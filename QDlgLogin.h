#ifndef QDLGLOGIN_H
#define QDLGLOGIN_H

#include <QDialog>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
namespace Ui {
class QDlgLogin;
}

class QDlgLogin : public QDialog
{
    Q_OBJECT

public:
    explicit QDlgLogin(int *userId, QWidget *parent = nullptr);
    ~QDlgLogin();

    int getUserId();


private slots:
    void on_btnLogin_clicked();

private:
    Ui::QDlgLogin *ui;

    QSqlDatabase db;
    QSqlQuery *query;
    QString m_user = "user";
    QString m_passwd = "12345";
    int *m_userId;
    void readSettings();
    void writeSettings();
    QString encrypt(const QString &str);
    void loadStyleSheet(const QString &styleSheetFile);

};

#endif // QDLGLOGIN_H
