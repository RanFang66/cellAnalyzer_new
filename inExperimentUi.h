#ifndef INEXPERIMENTUI_H
#define INEXPERIMENTUI_H

#include <QWidget>
#include <QMovie>
#include "ExperiCtrl.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

namespace Ui {
class inExperimentUi;
}

class inExperimentUi : public QWidget
{
    Q_OBJECT

public:
    explicit inExperimentUi(ExperiCtrl *experiCtrl, QWidget *parent = nullptr);
    ~inExperimentUi();

public slots:
    void updateNoticeText(QString notice);

    void onUpdateImage(void);


signals:
    void pauseExperiment();

private slots:
    void on_btnPauseExperi_clicked();

private:
    Ui::inExperimentUi *ui;

    QMovie *m_waitingGif;

    ExperiCtrl *experi;

    QGraphicsScene          *m_scene;
    QGraphicsPixmapItem     *m_image_item;

    void loadStyleSheet(const QString &styleSheetFile);
};

#endif // INEXPERIMENTUI_H
