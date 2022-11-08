#ifndef INEXPERIMENTUI_H
#define INEXPERIMENTUI_H

#include <QWidget>
#include <QMovie>

namespace Ui {
class inExperimentUi;
}

class inExperimentUi : public QWidget
{
    Q_OBJECT

public:
    explicit inExperimentUi(QWidget *parent = nullptr);
    ~inExperimentUi();

public slots:
    void updateNoticeText(QString notice);


signals:
    void pauseExperiment();

private slots:
    void on_btnPauseExperi_clicked();

private:
    Ui::inExperimentUi *ui;

    QMovie *m_waitingGif;

    void loadStyleSheet(const QString &styleSheetFile);
};

#endif // INEXPERIMENTUI_H
