#ifndef DLGANIMATIONUI_H
#define DLGANIMATIONUI_H

#include <QDialog>
#include <QMovie>

namespace Ui {
class DlgAnimationUi;
}

class DlgAnimationUi : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAnimationUi(QWidget *parent = nullptr);
    ~DlgAnimationUi();

private:
    Ui::DlgAnimationUi *ui;
    QMovie  *m_movie;
};

#endif // DLGANIMATIONUI_H
