#ifndef EXPERIDATAUI_H
#define EXPERIDATAUI_H

#include <QWidget>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QItemSelectionModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
namespace Ui {
class experiDataUi;
}

class experiDataUi : public QWidget
{
    Q_OBJECT

public:
    explicit experiDataUi(QWidget *parent = nullptr);
    ~experiDataUi();

signals:
    void showDataDetail(QString &experiID);

private slots:
    void on_btnDeleteData_clicked();
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_btnDetail_clicked();

private:
    Ui::experiDataUi *ui;

    QSqlDatabase db;
    QSqlQuery *query;
    QSqlQueryModel *qryModel;
    QItemSelectionModel *theSelection;
    QSqlRecord      curRec;
    QString         curExperiID;


    void initExperiDataUi();
    void loadStyleSheet(const QString &styleSheetFile);
};

#endif // EXPERIDATAUI_H
