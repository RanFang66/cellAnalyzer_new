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
    explicit exportDataDialog(QWidget *parent = nullptr);
    ~exportDataDialog();

private:
    Ui::exportDataDialog *ui;
};

#endif // EXPORTDATADIALOG_H
