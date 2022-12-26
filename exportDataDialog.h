#ifndef EXPORTDATADIALOG_H
#define EXPORTDATADIALOG_H

#include <QDialog>

namespace Ui {
class exportDataDialog;
}

class exportDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit exportDataDialog(int type, QString experiID, QWidget *parent = nullptr);
    ~exportDataDialog();

signals:
    void exportDataOk(bool);

private:
    Ui::exportDataDialog *ui;

    QString dataType;

    QString dataSavePath;

    QString executeShellCmd(QString strCmd, int timeout);

    QString detectUpan();

    bool createDir(QString &upan, QString &type);

    bool exportImageData(QString &experiID);
};

#endif // EXPORTDATADIALOG_H
