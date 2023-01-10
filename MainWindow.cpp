 #include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QDlgLogin.h>
#include <wificonfigui.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    loadStyleSheet(":/styles/main.qss");
    ui->setupUi(this);
//    QDlgLogin *dlgLogin = new QDlgLogin(&m_userId, this);


    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    QString ret = executeShellCmd("sudo raspi-gpio set 26 op dh && sleep 3");
    if (!ret.isEmpty()) {
        qDebug() << ret;
    }

    m_dev = new DevCtrl(this);
    m_setting = new ExperiSetting(this);
    m_data = new ExperiData(m_setting, this);
    m_experiCtrl = new ExperiCtrl(m_dev, m_setting, m_data, this);
    m_setting->setUserID(m_userId);

//    executeShellCmd("sleep 3");

    experiSetting = new experiSettingUi(m_setting, this);
    experiData = new experiDataUi(this);
    debugMode = new debugModeUi(m_dev, this);
//    inExperiment = new inExperimentUi(m_experiCtrl, this);
    experiRes = new experiResultUi(this);
    userManage = new UserManageUi(this);

    initMainWindowUi();
}

MainWindow::~MainWindow()
{
   executeShellCmd("sudo raspi-gpio set 26 op dl");
    delete ui;
}

void MainWindow::initMainWindowUi()
{
    experiSettingIndex = ui->stackedWidget->addWidget(experiSetting);
    experiDataIndex = ui->stackedWidget->addWidget(experiData);
    debugModeIndex = ui->stackedWidget->addWidget(debugMode);
//    inExperimentIndex = ui->stackedWidget->addWidget(inExperiment);
    experiResultIndex = ui->stackedWidget->addWidget(experiRes);
    userManageIndex = ui->stackedWidget->addWidget(userManage);

    ui->stackedWidget->setCurrentIndex(appSelcIndex);
    ui->btnExperiApp->setChecked(true);

    connect(experiSetting, SIGNAL(startExperiment()), this, SLOT(onExperimentStart()));
    connect(experiSetting, SIGNAL(exitExperimentSetting()), this, SLOT(onExitExperiSetting()));
//    connect(inExperiment, SIGNAL(pauseExperiment()), this, SLOT(onExperimentPaused()));
    connect(m_experiCtrl, SIGNAL(experimentFinished()), this, SLOT(onExperimentFinished()));
    connect(experiData, SIGNAL(showDataDetail(QString&)), this, SLOT(onShowDataDetail(QString&)));
    connect(experiRes, SIGNAL(returnToMainPage()), this, SLOT(onReturnToMainPage()));
//    connect(m_experiCtrl, SIGNAL(experiCapFinished()), inExperiment, SLOT(onUpdateImage()));
    connect(userManage, SIGNAL(return2SysSetting()), this, SLOT(onReturnSysSetting()));
    connect(m_dev, SIGNAL(chipStateUpdated(int)), experiSetting, SLOT(setChipState(int)));
}

QString MainWindow::executeShellCmd(QString strCmd)
{
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << strCmd);
    proc.waitForFinished(5000);
    QString strRet = proc.readAllStandardOutput();
    return strRet;
}


void MainWindow::on_btnSysSetting_clicked()
{
    ui->stackedWidget->setCurrentIndex(sysSettingIndex);
    mainIndex = sysSettingIndex;
}

void MainWindow::on_btnExperiData_clicked()
{
    if (mainIndex != experiDataIndex) {        
        ui->stackedWidget->setCurrentIndex(experiDataIndex);
        experiData->updateExperiDataUi();
        mainIndex = experiDataIndex;
    }
}

void MainWindow::on_btnExperiApp_clicked()
{
    if (mainIndex != appSelcIndex) {
        ui->stackedWidget->setCurrentIndex(appSelcIndex);
        mainIndex = appSelcIndex;
    }
}

void MainWindow::on_btnHelpDoc_clicked()
{
    if (mainIndex != helpDocIndex) {
        ui->stackedWidget->setCurrentIndex(helpDocIndex);
        mainIndex = helpDocIndex;
    }
}

void MainWindow::on_btnAOPI_clicked()
{
    m_setting->initSetting(m_userId, 1);
    experiSetting->initExperiSettingUi();
    ui->stackedWidget->setCurrentIndex(experiSettingIndex);
}

void MainWindow::on_btnDebugPage_clicked()
{
    if (mainIndex != debugModeIndex) {
        ui->stackedWidget->setCurrentIndex(debugModeIndex);
        mainIndex = debugModeIndex;
    }
}

void MainWindow::onSerialConnected(bool connected)
{
    if (connected) {
        qDebug() << "serial port: connected ok!";
    } else {
        QMessageBox::warning(this, "Error", "Serial port connected failed!");
    }
}

void MainWindow::onSerialDisconnected()
{
    qDebug() << "serial port: disconnected!";
}

void MainWindow::onCameraInitRet(bool camOk)
{
    if (camOk) {
        qDebug() << "Camera: initialized successfully";
    } else {
        QMessageBox::warning(this, "Error", "Camera init failed!");
    }
}

void MainWindow::onExperimentStart()
{
    ui->btnSysSetting->setEnabled(false);
    ui->btnExperiData->setEnabled(false);
    ui->btnExperiApp->setEnabled(false);
    ui->btnHelpDoc->setEnabled(false);
    ui->btnDebugPage->setEnabled(false);
    inExperiment = new inExperimentUi(m_experiCtrl, this);
    inExperiment->setAttribute(Qt::WA_DeleteOnClose);
    connect(inExperiment, SIGNAL(pauseExperiment()), this, SLOT(onExperimentPaused()));
    connect(m_experiCtrl, SIGNAL(experiCapFinished()), inExperiment, SLOT(onUpdateImage()));

    inExperimentIndex = ui->stackedWidget->addWidget(inExperiment);
    ui->stackedWidget->setCurrentIndex(inExperimentIndex);
//    inExperiment->initExperimentUi();
    m_experiCtrl->startExperiment(m_setting->getExperiId());
}

void MainWindow::onExperimentPaused()
{
    QMessageBox::StandardButton ret = QMessageBox::question(this, "Pause Experiment", "Sure to pause this experiment?");

    if (ret == QMessageBox::Yes) {
        ui->btnSysSetting->setEnabled(true);
        ui->btnExperiData->setEnabled(true);
        ui->btnExperiApp->setEnabled(true);
        ui->btnHelpDoc->setEnabled(true);
        ui->btnDebugPage->setEnabled(true);
        m_experiCtrl->pauseExperiment();
        ui->stackedWidget->setCurrentIndex(appSelcIndex);
        delete inExperiment;
    }
}

void MainWindow::onExitExperiSetting()
{
    ui->stackedWidget->setCurrentIndex(appSelcIndex);
}

void MainWindow::onExperimentFinished()
{
    ui->btnSysSetting->setEnabled(true);
    ui->btnExperiData->setEnabled(true);
    ui->btnExperiApp->setEnabled(true);
    ui->btnHelpDoc->setEnabled(true);
    ui->btnDebugPage->setEnabled(true);
    experiRes->initResultShow(m_setting->getExperiId());
    ui->stackedWidget->setCurrentIndex(experiResultIndex);
    delete inExperiment;

}

void MainWindow::onShowDataDetail(QString &id)
{
    experiRes->initResultShow(id);
    ui->stackedWidget->setCurrentIndex(experiResultIndex);
}

void MainWindow::onReturnToMainPage()
{
    ui->stackedWidget->setCurrentIndex(mainIndex);
}


void MainWindow::loadStyleSheet(const QString &styleSheetFile)
{
    QFile file(styleSheetFile);
    file.open(QFile::ReadOnly);
    if (file.isOpen()) {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Login: Open Style Sheet File Failed!";
    }
}

void MainWindow::on_btnUserManage_clicked()
{
    ui->stackedWidget->setCurrentIndex(userManageIndex);
}

void MainWindow::onReturnSysSetting()
{
    ui->stackedWidget->setCurrentIndex(sysSettingIndex);
}

void MainWindow::on_btnLanguage_clicked()
{
    ui->stackedWidget->setCurrentIndex(languageSelcIndex);
}

void MainWindow::on_btnShutdown_clicked()
{
    QMessageBox::StandardButton ret = QMessageBox::question(this, "shutdown", "Sure to shutdown?");
    if (ret == QMessageBox::Yes) {
        qApp->exit(0);
    } else {
        return;
    }
}

void MainWindow::on_btnWifiConfig_clicked()
{
    WifiConfigUi *wifi = new WifiConfigUi(this);
    wifi->setAttribute(Qt::WA_DeleteOnClose);
    int wifiIndex = ui->stackedWidget->addWidget(wifi);
    ui->stackedWidget->setCurrentIndex(wifiIndex);
    connect(wifi, &WifiConfigUi::return2SysSetting, this, &MainWindow::onReturnSysSetting);
}

