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
    void setChipState(int state);
private slots:
    void on_btnOk_clicked();

private:
    Ui::DlgAnimationUi *ui;
    QMovie  *m_movie;
    bool m_chipState;
};

inline void DlgAnimationUi::setChipState(int state)
{
    m_chipState = state;
}

#endif // DLGANIMATIONUI_H
