#include "debugModeUi.h"
#include "ui_debugModeUi.h"
#include <QMessageBox>
debugModeUi::debugModeUi(DevCtrl *dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::debugModeUi),
    m_dev(dev)
{
    ui->setupUi(this);
    initDubugModeUi();
}

debugModeUi::~debugModeUi()
{
    delete ui;
}

void debugModeUi::onCamImageUpdated()
{
    QImage imgShow = m_dev->getQImage();
    double clarity = m_dev->getClarity();
    if (m_image_item) {
        m_image_item->setPixmap(QPixmap::fromImage(imgShow));
    }
    m_scene->setSceneRect(0, 0, imgShow.width(), imgShow.height());
    ui->lblClarity->setText(QString::number(clarity));
}

void debugModeUi::onDevStatusUpdated()
{
    int pos = m_dev->getMotorPos(DevCtrl::CHIP_MOTOR_X);
    int limit = m_dev->getMotorLimitState(DevCtrl::CHIP_MOTOR_X);
    ui->tblSysStatus->item(0, 0)->setText(QString::number(pos));
    ui->tblSysStatus->item(0, 1)->setText(QString::number(limit));
    pos = m_dev->getMotorPos(DevCtrl::CHIP_MOTOR_Y);
    limit = m_dev->getMotorLimitState(DevCtrl::CHIP_MOTOR_X);
    ui->tblSysStatus->item(1, 0)->setText(QString::number(pos));
    ui->tblSysStatus->item(1, 1)->setText(QString::number(limit));
    pos = m_dev->getMotorPos(DevCtrl::CAMERA_MOTOR);
    limit = m_dev->getMotorLimitState(DevCtrl::CAMERA_MOTOR);
    ui->tblSysStatus->item(2, 0)->setText(QString::number(pos));
    ui->tblSysStatus->item(2, 1)->setText(QString::number(limit));
    pos = m_dev->getMotorPos(DevCtrl::FILTER_MOTOR);
    limit = m_dev->getMotorLimitState(DevCtrl::FILTER_MOTOR);
    ui->tblSysStatus->item(3, 0)->setText(QString::number(pos));
    ui->tblSysStatus->item(3, 1)->setText(QString::number(limit));
}

void debugModeUi::onAutoFocusComplete()
{
    onDevStatusUpdated();
    onCamImageUpdated();
    QMessageBox::about(this, "Complete", "Auto focus completeed");
}

void debugModeUi::onLedChanged()
{
    if (ui->rBtnLedWhite->isChecked()) {
        m_dev->ledLigthOn(DevCtrl::LED_WHITE);
    } else if (ui->rBtnLedBlue->isChecked()) {
        m_dev->ledLigthOn(DevCtrl::LED_BLUE);
    } else if (ui->rBtnLedGreen->isChecked()) {
        m_dev->ledLigthOn(DevCtrl::LED_GREEN);
    } else {
        m_dev->ledLightOff();
    }
}

void debugModeUi::onMotorChanged()
{
    if (ui->rBtnChipMotorX->isChecked()) {
        m_motorId = DevCtrl::CHIP_MOTOR_X;
    } else if (ui->rBtnChipMotorY->isChecked()) {
        m_motorId = DevCtrl::CHIP_MOTOR_Y;
    } else if (ui->rBtnCamMotor->isChecked()) {
        m_motorId = DevCtrl::CAMERA_MOTOR;
    } else if (ui->rBtnFilterMotor->isChecked()) {
        m_motorId = DevCtrl::FILTER_MOTOR;
    }
}

void debugModeUi::initDubugModeUi()
{
    m_scene = new QGraphicsScene(this);
    ui->gvMain->setScene(m_scene);

    if (m_dev->getSerialState()) {
        ui->lblSerialState->setText(tr("Serial Connected"));
    } else {
        ui->lblSerialState->setText(tr("Serial Disconneted"));
    }

    if (m_dev->getCameraState()) {
        ui->lblCamName->setText(tr("Camera Open"));
        m_image_item = m_scene->addPixmap(QPixmap(600, 1000));
    } else {
        ui->lblCamName->setText(tr("Camera Closed"));
    }

    connect(m_dev, SIGNAL(imageUpdated()), this, SLOT(onCamImageUpdated()));
    connect(m_dev, SIGNAL(devStatusUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(chipXMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(chipYMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(cameraMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(filterMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));
    connect(m_dev, SIGNAL(autoFocusComplete()), this, SLOT(onAutoFocusComplete()));
    connect(ui->rBtnLedGreen, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnLedBlue, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnLedWhite, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnLedOff, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnChipMotorX, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
    connect(ui->rBtnChipMotorY, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
    connect(ui->rBtnCamMotor, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
    connect(ui->rBtnFilterMotor, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
    updateCamParas();
}

void debugModeUi::zoomIn()
{
    ui->gvMain->scale(1.2, 1.2);
}

void debugModeUi::zoomOut()
{
    ui->gvMain->scale(1/1.2, 1/1.2);
}

void debugModeUi::resetView()
{
    ui->gvMain->resetTransform ();
}

void debugModeUi::updateCamParas()
{
    double gamma;
    CameraGetGamma(CAM_INDEX_0, &gamma);
    ui->lblGamma->setText(QString("%1").arg(gamma));
    ui->hsGamma->setValue(gamma*100);

    double contrast;
    CameraGetContrast(CAM_INDEX_0, &contrast);
    ui->lblContrast->setText(QString("%1").arg(contrast));
    ui->hsContrast->setValue(contrast*100);

    bool aec,agc;
    CameraGetAGC(CAM_INDEX_0, &agc);
    CameraGetAEC(CAM_INDEX_0, &aec);

    if(agc)
    {
        ui->hsGain->setEnabled(false);
        ui->cbGain->setChecked(true);
    }
    else
    {
        int gain;
        CameraGetGain(CAM_INDEX_0, &gain);
        ui->lblGain->setText(QString("%1").arg(gain));
        ui->hsGain->setValue(gain);
        ui->hsGain->setEnabled(true);
        ui->cbExposure->setChecked(true);
        ui->cbGain->setChecked(false);
    }


    if(aec)
    {
        ui->hsExposure->setEnabled(false);
        ui->cbExposure->setChecked(true);
    }
    else
    {
        int exposure;
        CameraGetExposure(CAM_INDEX_0, &exposure);
        ui->lblExposure->setText(QString("%1").arg(exposure));
        ui->hsExposure->setValue(exposure);
        ui->hsExposure->setEnabled(true);
        ui->cbExposure->setChecked(false);
    }
}

void debugModeUi::on_btnMotoRunForward_clicked()
{
    m_dev->motorRun(m_motorId, DevCtrl::MOTOR_RUN_FORWARD);
}

void debugModeUi::on_btnMotoRunBackward_clicked()
{
    m_dev->motorRun(m_motorId, DevCtrl::MOTOR_RUN_BACKWARD);
}

void debugModeUi::on_btnMotoStop_clicked()
{
    m_dev->motorStop(m_motorId);
}

void debugModeUi::on_btnMotoReset_clicked()
{
    m_dev->motorReset(m_motorId);
}

void debugModeUi::on_btnMotoSteps_clicked()
{
    int pos = ui->editMotoRunSteps->text().toInt();
    m_dev->motorRun(m_motorId, DevCtrl::MOTOR_RUN_POS, pos);
}

void debugModeUi::on_btnSetMotoSpeed_clicked()
{
    int speed = ui->editMotoSpeed->text().toInt();
    m_dev->motorSetSpeed(m_motorId, speed);
}

void debugModeUi::on_btnCamRun_clicked()
{
    m_dev->cameraRun();

}

void debugModeUi::on_btnCamStop_clicked()
{
    m_dev->cameraStop();
}

void debugModeUi::on_btnUpdateSysStatus_clicked()
{
    m_dev->updateDevStatus();
}

void debugModeUi::on_btnAutoFocus_clicked(bool checked)
{
//    ui->btnAutoFocus->setDisabled(true);
    m_dev->startAutoFocus(checked);
}

void debugModeUi::on_btnChipIn_clicked()
{
    m_dev->motorRun(DevCtrl::INSERT_MOTOR, 1);
}


void debugModeUi::on_btnChipOut_clicked()
{
    m_dev->motorRun(DevCtrl::INSERT_MOTOR, 2);
}

void debugModeUi::on_btnChipStop_clicked()
{
    m_dev->motorRun(DevCtrl::INSERT_MOTOR, 0);
}

void debugModeUi::on_cmbResolution_currentIndexChanged(int index)
{
    m_dev->camChangeResolution(index);
}

void debugModeUi::on_btnZoomIn_clicked()
{
    zoomIn();
}

void debugModeUi::on_btnZoomOut_clicked()
{
    zoomOut();
}

void debugModeUi::on_btnResetView_clicked()
{
    resetView();
}

void debugModeUi::on_btnUpdateResolution_clicked()
{
    int width = 0, height = 0;
    int cnt = m_dev->getCamResolutionsCount();
    ui->cmbResolution->clear();
    for (int i = 0 ; i < cnt; i++) {
        m_dev->getCamResolution(i, width, height);
        QString res = QString::asprintf("%d x %d", width, height);
        ui->cmbResolution->addItem(res, i);
    }
}

void debugModeUi::on_cbGain_clicked(bool checked)
{
    CameraSetAGC(0, checked);
    updateCamParas();
    ui->hsGain->setDisabled(checked);
}

void debugModeUi::on_hsGain_valueChanged(int value)
{
    CameraSetGain(0, value);
    ui->lblGain->setText(QString::number(value));

}

void debugModeUi::on_hsExposure_valueChanged(int value)
{
    CameraSetExposure(0, value);
    ui->lblExposure->setText(QString::number(value));
}

void debugModeUi::on_hsContrast_valueChanged(int value)
{
    double contrast = value / 100.0;
    CameraSetContrast(0, contrast);
    ui->lblContrast->setText(QString::number(value));
}

void debugModeUi::on_hsGamma_valueChanged(int value)
{
    double gamma = value / 100.0;
    CameraSetGamma(0, gamma);
    ui->lblGamma->setText(QString::number(value));
}

void debugModeUi::on_cbExposure_clicked(bool checked)
{
    m_dev->cameraAutoExplosure(checked);
    updateCamParas();
    ui->hsExposure->setDisabled(checked);
}

void debugModeUi::on_btnWB_clicked()
{
    m_dev->cameraWhiteBalance();
    updateCamParas();
}
