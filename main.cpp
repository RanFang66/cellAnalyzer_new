#include "MainWindow.h"

#include <QApplication>
#include <QSql>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDlgLogin.h>
#include <QMessageBox>
bool createDbConnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "cellDataConn");
    db.setDatabaseName("cellAnalyzerDb.db");
    bool ok = db.open();
    if (!ok) {
        qDebug() << ": open database error!" + db.lastError().text();
        return false;
    } else {
        qDebug() << ": open database successfully!";
        qDebug() << db.tables();
        return true;
    }

}


int main(int argc, char *argv[])
{

    if (createDbConnect()) {
        QApplication a(argc, argv);
        MainWindow w;
        QDlgLogin *dlgLogin = new QDlgLogin();

        w.show();
        if (dlgLogin->exec() == QDialog::Accepted) {
            return a.exec();
        } else {
            return 0;
        }
        return a.exec();
    } else {
        qDebug()<< "-------System Error: Can not open database!";
    }
}
