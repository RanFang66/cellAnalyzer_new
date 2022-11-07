#ifndef DEBUGMODEUI_H
#define DEBUGMODEUI_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
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
    void onDevStatusUpdated();
    void onAutoFocusComplete();

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

    void on_cbExposure_clicked(bool checked);

    void on_btnWB_clicked();

    void on_btnAutoFocus_clicked(bool checked);

    void on_btnChipIn_clicked();

    void on_btnChipOut_clicked();

    void on_btnChipStop_clicked();

    void on_cmbResolution_currentIndexChanged(int index);

    void on_btnZoomIn_clicked();

    void on_btnZoomOut_clicked();

    void on_btnResetView_clicked();

    void on_btnUpdateResolution_clicked();

    void on_cbGain_clicked(bool checked);

    void on_hsGain_valueChanged(int value);

    void on_hsExposure_valueChanged(int value);

    void on_hsContrast_valueChanged(int value);

    void on_hsGamma_valueChanged(int value);

private:
    Ui::debugModeUi         *ui;
    DevCtrl                 *m_dev;
    QGraphicsScene          *m_scene;
    QGraphicsPixmapItem     *m_image_item;

    int  m_motorId;

    void initDubugModeUi();

    void zoomIn();

    void zoomOut();

    void resetView();

    void updateCamParas();
};

#endif // DEBUGMODEUI_H
