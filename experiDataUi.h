#ifndef EXPERIDATAUI_H
#define EXPERIDATAUI_H

#include <QWidget>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QItemSelectionModel>
namespace Ui {
class experiDataUi;
}

class experiDataUi : public QWidget
{
    Q_OBJECT

public:
    explicit experiDataUi(QWidget *parent = nullptr);
    ~experiDataUi();

private:
    Ui::experiDataUi *ui;

    QSqlDatabase db;
    QSqlQuery *query;
    QSqlTableModel *tblModel;
    QItemSelectionModel *theSelection;

    void initExperiDataUi();
};

#endif // EXPERIDATAUI_H
