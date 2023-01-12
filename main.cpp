#include "MainWindow.h"
#include <QWindow>
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
//    db.setDatabaseName("/home/seekgene/gitRepo/cellAnalyzer_new/cellAnalyzerDb.db");
    db.setDatabaseName("/home/ran/gitRepos/cellAnalyzer_new/cellAnalyzerDb.db");

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

static void handleVisibleChanged() {
    if (!QGuiApplication::inputMethod()->isVisible())
        return;
    for (QWindow * w: QGuiApplication::allWindows()) {
        if (std::strcmp(w->metaObject()->className(), "QtVirtualKeyboard::InputView") == 0) {
            if (QObject *keyboard = w->findChild<QObject *>("keyboard")) {
                QRect r = w->geometry();
                r.moveTop(keyboard->property("y").toDouble());
                w->setMask(r);
                return;
            }
        }
    }
}
int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    if (createDbConnect()) {
        QApplication a(argc, argv);
//        QApplication::setOverrideCursor(Qt::BlankCursor);
        MainWindow w;
        QDlgLogin *dlgLogin = new QDlgLogin(w.getUserIdPointer(), &w);

        QObject::connect(QGuiApplication::inputMethod(), &QInputMethod::visibleChanged, &handleVisibleChanged);


        w.show();
        dlgLogin->show();
        return a.exec();



//        if (dlgLogin->exec() == QDialog::Accepted) {
//            w.show();
//            return a.exec();
//        } else {
//            return 0;
//        }
    } else {
        qDebug()<< "-------System Error: Can not open database!";
    }
}
