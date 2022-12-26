#ifndef EXPERIDATAUI_H
#define EXPERIDATAUI_H

#include <QWidget>

#include <QSqlDatabase>
#include <QProcess>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QItemSelectionModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QDir>
namespace Ui {
class experiDataUi;
}

class experiDataUi : public QWidget
{
    Q_OBJECT

public:
    explicit experiDataUi(QWidget *parent = nullptr);
    ~experiDataUi();
    void updateExperiDataUi();
    enum EXPORT_DATA_TYPE {
        EXPORT_IMAGE = 1,
        EXPORT_PDF,
        EXPORT_CSV,
    };

signals:
    void showDataDetail(QString &experiID);

private slots:
    void on_btnDeleteData_clicked();
    void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_btnDetail_clicked();

    void on_btnPDF_clicked();

    void on_btnExcel_clicked();

    void on_btnJPG_clicked();

    void on_btnFirstPage_clicked();

    void on_btnPrevPage_clicked();

    void on_btnNextPage_clicked();

    void on_spinPageNum_valueChanged(int arg1);

    void on_btnLastPage_clicked();

private:
    Ui::experiDataUi *ui;

    QSqlDatabase db;
    QSqlQuery *query;
    QSqlQueryModel *qryModel;
    QItemSelectionModel *theSelection;
    QSqlRecord      curRec;
    QString         curExperiID;

    bool    m_exportDataFlag = false;
    int     m_exportType;
    int     m_pageIndex;
    int     m_recordsPerPage;
    int     m_recordsNum;
    void initExperiDataUi();
    void loadStyleSheet(const QString &styleSheetFile);
    QString executeShellCmd(QString strCmd, int timeout);
    QString detectUpan();

};

#endif // EXPERIDATAUI_H
