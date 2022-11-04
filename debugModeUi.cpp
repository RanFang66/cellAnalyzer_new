#include "debugModeUi.h"
#include "ui_debugModeUi.h"

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
    ui->btnAutoFocus->setDisabled(false);
    onDevStatusUpdated();
    onCamImageUpdated();
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
    connect(m_dev, SIGNAL(autoFocusComplete()), this, SLOT(onAutoFocusComplete()));
    connect(ui->rBtnLedGreen, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnLedBlue, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnLedWhite, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnLedOff, SIGNAL(clicked()), this, SLOT(onLedChanged()));
    connect(ui->rBtnChipMotorX, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
    connect(ui->rBtnChipMotorY, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
    connect(ui->rBtnCamMotor, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
    connect(ui->rBtnFilterMotor, SIGNAL(clicked()), this, SLOT(onMotorChanged()));
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

void debugModeUi::on_cbExposure_3_clicked(bool checked)
{
    m_dev->cameraAutoExplosure(checked);
}

void debugModeUi::on_btnWB_clicked()
{
    m_dev->cameraWhiteBalance();
}

void debugModeUi::on_btnAutoFocus_clicked(bool checked)
{
    ui->btnAutoFocus->setDisabled(true);
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
