#include "debugModeUi.h"
#include "ui_debugModeUi.h"
#include <QMessageBox>
#include <JHCap.h>
#include <QSqlError>
#include <QDebug>
debugModeUi::debugModeUi(DevCtrl *dev, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::debugModeUi),
    m_dev(dev)
{
    ui->setupUi(this);
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);

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
    bool chip = m_dev->getChipState();
    QString chipString;
    if (chip) {
        chipString = "with chip";
    } else {
        chipString = "No chip";
    }
    ui->tblSysStatus->item(5, 0)->setText(chipString);
}

void debugModeUi::onAutoFocusComplete()
{
    onDevStatusUpdated();
    disconnect(m_dev, SIGNAL(autoFocusComplete()), this, SLOT(onAutoFocusComplete()));
    m_dev->cameraRun();
    QMessageBox::about(this, "Complete", "Auto focus completeed");
}

void debugModeUi::onLedChanged()
{
    if (ui->rBtnLedWhite->isChecked()) {
        m_dev->ledLigthOn(DevCtrl::LED_WHITE);
        m_dev->initCameraParas(1);
        updateCamParas();
    } else if (ui->rBtnLedBlue->isChecked()) {
        m_dev->ledLigthOn(DevCtrl::LED_BLUE);
        m_dev->initCameraParas(2);
        updateCamParas();
    } else if (ui->rBtnLedGreen->isChecked()) {
        m_dev->ledLigthOn(DevCtrl::LED_GREEN);
        m_dev->initCameraParas(3);
        updateCamParas();
    } else {
        m_dev->ledLightOff();
        m_dev->initCameraParas(1);
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


    connect(m_dev, SIGNAL(devStatusUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(chipXMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(chipYMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(cameraMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));
//    connect(m_dev, SIGNAL(filterMotorStateUpdated()), this, SLOT(onDevStatusUpdated()));

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

    double sat;
    CameraGetSaturation(CAM_INDEX_0, &sat);
    int satInt = sat * 100;
    ui->lblSaturation->setText(QString::number(sat));
    ui->hsSaturation->setValue(satInt);

    int  blackLevel;
    CameraGetBlackLevel(CAM_INDEX_0, &blackLevel);
    ui->lblBlackBalance->setText(QString::number(blackLevel));
    ui->hsBlackBalance->setValue(blackLevel);

    bool aec,agc, awb;
    CameraGetAGC(CAM_INDEX_0, &agc);
    CameraGetAEC(CAM_INDEX_0, &aec);
    CameraGetAWB(CAM_INDEX_0, &awb);

    if(agc)
    {
        int gain;
        CameraGetGain(CAM_INDEX_0, &gain);
        ui->lblGain->setText(QString("%1").arg(gain));
        ui->hsGain->setValue(gain);
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
        int exposure, aet;
        CameraGetExposure(CAM_INDEX_0, &exposure);
        ui->lblExposure->setText(QString("%1").arg(exposure));
        ui->hsExposure->setValue(exposure);
        ui->hsExposure->setEnabled(false);
        ui->cbExposure->setChecked(true);
        ui->hsAutoExpoTarget->setEnabled(true);
        CameraGetAETarget(CAM_INDEX_0, &aet);
        ui->hsAutoExpoTarget->setValue(aet);
        ui->lblAETarget->setText(QString::number(aet));
    }
    else
    {
        int exposure, aet;
        CameraGetExposure(CAM_INDEX_0, &exposure);
        ui->lblExposure->setText(QString("%1").arg(exposure));
        ui->hsExposure->setValue(exposure);
//        CameraGetAETarget(CAM_INDEX_0, &aet);
//        ui->hsAutoExpoTarget->setValue(aet);
//        ui->lblAETarget->setText(QString::number(aet));
        ui->hsExposure->setEnabled(true);
        ui->cbExposure->setChecked(false);
        ui->hsAutoExpoTarget->setEnabled(false);
    }

    if (awb) {
        ui->hsRedGain->setEnabled(false);
        ui->hsBlueGain->setEnabled(false);
        ui->hsGreenGain->setEnabled(false);
        ui->cbAutoWB->setChecked(true);
    } else {
        double rg, gg, bg;
        ui->cbAutoWB->setChecked(false);
        ui->hsRedGain->setEnabled(true);
        ui->hsBlueGain->setEnabled(true);
        ui->hsGreenGain->setEnabled(true);
        CameraGetWBGain(CAM_INDEX_0, &rg, &gg, &bg);
        ui->hsRedGain->setValue(rg*100);
        ui->hsBlueGain->setValue(bg*100);
        ui->hsGreenGain->setValue(gg*100);
        ui->lblRedGain->setText(QString::number(rg));
        ui->lblGreenGain->setText(QString::number(gg));
        ui->lblBlueGain->setText(QString::number(bg));
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
 //   m_dev->connectCamera();
    m_dev->cameraRun();
    connect(m_dev, SIGNAL(imageUpdated()), this, SLOT(onCamImageUpdated()));

}

void debugModeUi::on_btnCamStop_clicked()
{
    m_dev->cameraStop();
    disconnect(m_dev, SIGNAL(imageUpdated()), this, SLOT(onCamImageUpdated()));
//    m_dev->disconnectCamera();
}

void debugModeUi::on_btnUpdateSysStatus_clicked()
{
    m_dev->updateDevStatus();
}

void debugModeUi::on_btnAutoFocus_clicked(bool checked)
{
//    ui->btnAutoFocus->setDisabled(true);
    connect(m_dev, SIGNAL(autoFocusComplete()), this, SLOT(onAutoFocusComplete()));
    if (checked)
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
    ui->lblContrast->setText(QString::number(contrast));
}

void debugModeUi::on_hsGamma_valueChanged(int value)
{
    double gamma = value / 100.0;
    CameraSetGamma(0, gamma);
    ui->lblGamma->setText(QString::number(gamma));
}

void debugModeUi::on_cbExposure_clicked(bool checked)
{
    CameraSetAEC(CAM_INDEX_0, checked);
    updateCamParas();
    ui->hsExposure->setDisabled(checked);
}

void debugModeUi::on_btnWB_clicked()
{
    CameraOnePushWB(CAM_INDEX_0);
    updateCamParas();
}

void debugModeUi::on_hsSaturation_valueChanged(int value)
{
    double sat = value / 100.0;
    CameraSetSaturation(0, sat);
    ui->lblSaturation->setText(QString::number(sat));
}

void debugModeUi::on_hsBlackBalance_valueChanged(int value)
{
    CameraSetBlackLevel(0, value);
    ui->lblBlackBalance->setText(QString::number(value));
}

void debugModeUi::on_cbAutoWB_clicked(bool checked)
{
    CameraSetAWB(0, checked);
    ui->hsRedGain->setDisabled(checked);
    ui->hsGreenGain->setDisabled(checked);
    ui->hsBlueGain->setDisabled(checked);
    updateCamParas();
}


void debugModeUi::on_hsAutoExpoTarget_valueChanged(int value)
{
    CameraSetAETarget(0, value);
    ui->lblAETarget->setText(QString::number(value));
}

void debugModeUi::on_btnBrCamParas_clicked()
{
    m_dev->initCameraParas(1);
    updateCamParas();
}



void debugModeUi::on_btnFL2CamParas_clicked()
{
    m_dev->initCameraParas(2);
    updateCamParas();
}

void debugModeUi::on_btnFL1CamParas_clicked()
{
    m_dev->initCameraParas(3);
    updateCamParas();
}

void debugModeUi::on_hsRedGain_valueChanged(int value)
{
    double rg = value / 100.0;
    double gg = ui->hsGreenGain->value() / 100.0;
    double bg = ui->hsBlueGain->value() /100.0;

    ui->lblRedGain->setText(QString::number(rg));
    CameraSetWBGain(CAM_INDEX_0, rg, gg, bg);
}

void debugModeUi::on_hsGreenGain_valueChanged(int value)
{
    double gg = value / 100.0;
    double rg = ui->hsRedGain->value() / 100.0;
    double bg = ui->hsBlueGain->value() /100.0;

    ui->lblGreenGain->setText(QString::number(gg));
    CameraSetWBGain(CAM_INDEX_0, rg, gg, bg);
}

void debugModeUi::on_hsBlueGain_valueChanged(int value)
{
    double bg = value / 100.0;
    double rg = ui->hsRedGain->value() / 100.0;
    double gg = ui->hsGreenGain->value() / 100.0;

    ui->lblBlueGain->setText(QString::number(bg));
    CameraSetWBGain(CAM_INDEX_0, rg, gg, bg);
}

void debugModeUi::on_btnSaveBrParas_clicked()
{
    double gamma;
    gamma = ui->hsGamma->value() / 100.0;

    double contrast;
    contrast = ui->hsContrast->value() / 100.0;

    double sat;
    sat = ui->hsSaturation->value() / 100.0;

    int  blackLevel;
    blackLevel = ui->hsBlackBalance->value();

    int aec, agc, awb;
    aec = ui->cbExposure->isChecked();
    agc = ui->cbGain->isChecked();
    awb = ui->cbAutoWB->isChecked();

    int gain = ui->hsGain->value();
    int exposure = ui->hsExposure->value();
    int aeTarget = ui->hsAutoExpoTarget->value();

    double rg = ui->hsRedGain->value() / 100.0;
    double gg = ui->hsGreenGain->value() / 100.0;
    double bg = ui->hsBlueGain->value() / 100.0;


    QString queryStr = QString("UPDATE camParas SET autoExpo=%1, autoGain=%2, autoWhiteBalance=%3, expoTime=%4, "
                               "expoGain=%5, autoExpoTarget=%6, gamma=%7, contrast=%8, satruation=%9, blackGain=%10,"
                               "wbRedGain=%11, wbGreenGain=%12, wbBlueGain=%13 WHERE ParaId = 1").arg(aec).arg(agc) \
            .arg(awb).arg(exposure).arg(gain).arg(aeTarget).arg(gamma).arg(contrast).arg(sat) \
            .arg(blackLevel).arg(rg).arg(gg).arg(bg);

    if (query->exec(queryStr)) {
        QMessageBox::information(this, "save ok", "save bright parameters ok");
    } else {
        qDebug() << queryStr;
        qDebug() << query->lastError().text();
        QMessageBox::critical(this, "save failed", "save bright parameters failed");
    }
}


void debugModeUi::on_btnSaveFL1Paras_clicked()
{
    double gamma;
    gamma = ui->hsGamma->value() / 100.0;

    double contrast;
    contrast = ui->hsContrast->value() / 100.0;

    double sat;
    sat = ui->hsSaturation->value() / 100.0;

    int  blackLevel;
    blackLevel = ui->hsBlackBalance->value();

    int aec, agc, awb;
    aec = ui->cbExposure->isChecked();
    agc = ui->cbGain->isChecked();
    awb = ui->cbAutoWB->isChecked();

    int gain = ui->hsGain->value();
    int exposure = ui->hsExposure->value();
    int aeTarget = ui->hsAutoExpoTarget->value();

    double rg = ui->hsRedGain->value() / 100.0;
    double gg = ui->hsGreenGain->value() / 100.0;
    double bg = ui->hsBlueGain->value() / 100.0;


    QString queryStr = QString("UPDATE camParas SET autoExpo=%1, autoGain=%2, autoWhiteBalance=%3, expoTime=%4, "
                               "expoGain=%5, autoExpoTarget=%6, gamma=%7, contrast=%8, satruation=%9, blackGain=%10,"
                               "wbRedGain=%11, wbGreenGain=%12, wbBlueGain=%13 WHERE ParaId = 2").arg(aec).arg(agc) \
            .arg(awb).arg(exposure).arg(gain).arg(aeTarget).arg(gamma).arg(contrast).arg(sat) \
            .arg(blackLevel).arg(rg).arg(gg).arg(bg);

    if (query->exec(queryStr)) {
        QMessageBox::information(this, "save ok", "save FL_green parameters ok");
    } else {
        QMessageBox::critical(this, "save failed", "save FL_green parameters failed");
    }
}


void debugModeUi::on_btnSaveFL2Paras_clicked()
{
    double gamma;
    gamma = ui->hsGamma->value() / 100.0;

    double contrast;
    contrast = ui->hsContrast->value() / 100.0;

    double sat;
    sat = ui->hsSaturation->value() / 100.0;

    int  blackLevel;
    blackLevel = ui->hsBlackBalance->value();

    int aec, agc, awb;
    aec = ui->cbExposure->isChecked();
    agc = ui->cbGain->isChecked();
    awb = ui->cbAutoWB->isChecked();

    int gain = ui->hsGain->value();
    int exposure = ui->hsExposure->value();
    int aeTarget = ui->hsAutoExpoTarget->value();

    double rg = ui->hsRedGain->value() / 100.0;
    double gg = ui->hsGreenGain->value() / 100.0;
    double bg = ui->hsBlueGain->value() / 100.0;


    QString queryStr = QString("UPDATE camParas SET autoExpo=%1, autoGain=%2, autoWhiteBalance=%3, expoTime=%4, "
                               "expoGain=%5, autoExpoTarget=%6, gamma=%7, contrast=%8, satruation=%9, blackGain=%10,"
                               "wbRedGain=%11, wbGreenGain=%12, wbBlueGain=%13 WHERE ParaId = 3").arg(aec).arg(agc) \
            .arg(awb).arg(exposure).arg(gain).arg(aeTarget).arg(gamma).arg(contrast).arg(sat) \
            .arg(blackLevel).arg(rg).arg(gg).arg(bg);

    if (query->exec(queryStr)) {
        QMessageBox::information(this, "save ok", "save FL_red parameters ok");
    } else {
        QMessageBox::critical(this, "save failed", "save FL_red parameters failed");
    }
}


void debugModeUi::on_lblModifyParas_clicked()
{
    int low, high, step;


    high = ui->spinAFHighLimit->value();
    low = ui->spinAFLowLimit->value();
    step = ui->spinAFStep->value();
    if (high <= low) {
        QMessageBox::warning(this, "错误", "高位置不能小于低位置");
        return;
    }
    QString queryStr = QString("UPDATE autoFocusPara SET startPos=%1, endPos=%2, step=%3 WHERE ID = 1").arg(low).arg(high).arg(step);
    if (query->exec(queryStr)) {
        QMessageBox::information(this, "修改成功", "修改自动对焦参数成功");
    } else {
        QMessageBox::critical(this, "修改失败", "修改自动对焦参数失败");
    }

//    emit setAutoFocusParameters(low, high, step);
}

