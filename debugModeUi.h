#ifndef DEBUGMODEUI_H
#define DEBUGMODEUI_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <DevCtrl.h>


namespace Ui {
class debugModeUi;
}

class debugModeUi : public QWidget
{
    Q_OBJECT

public:
    explicit debugModeUi(DevCtrl *dev, QWidget *parent = nullptr);
    ~debugModeUi();

public slots:
    void onCamImageUpdated();
    void onTimerTimeout();
    void onDevStatusUpdated();

private slots:
    void onLedChanged();

    void onMotorChanged();

    void on_btnMotoRunForward_clicked();

    void on_btnMotoRunBackward_clicked();

    void on_btnMotoStop_clicked();

    void on_btnMotoReset_clicked();

    void on_btnMotoSteps_clicked();

    void on_btnSetMotoSpeed_clicked();

    void on_btnCamRun_clicked();

    void on_btnCamStop_clicked();

    void on_btnUpdateSysStatus_clicked();

private:
    Ui::debugModeUi *ui;
    DevCtrl         *m_dev;
    QTimer          *camTimer;
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_image_item;

    int  m_motorId;

    void initDubugModeUi();
};

#endif // DEBUGMODEUI_H
