#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString ret = executeShellCmd("sudo raspi-gpio set 26 op dh");
    if (ret.isEmpty()) {
        QString ret2 = executeShellCmd("sleep 3");
    } else {
        qDebug() << ret;
    }

    loadStyleSheet(":/styles/main.qss");
    m_dev = new DevCtrl(this);

    executeShellCmd("sleep 3");
    experiSetting = new experiSettingUi(this);
    experiData = new experiDataUi(this);
    debugMode = new debugModeUi(m_dev, this);
    inExperiment = new inExperimentUi(this);

    initMainWindowUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMainWindowUi()
{
    experiSettingIndex = ui->stackedWidget->addWidget(experiSetting);
    experiDataIndex = ui->stackedWidget->addWidget(experiData);
    debugModeIndex = ui->stackedWidget->addWidget(debugMode);
    inExperimentIndex = ui->stackedWidget->addWidget(inExperiment);
    ui->stackedWidget->setCurrentIndex(appSelcIndex);
    ui->btnExperiApp->setChecked(true);

    connect(experiSetting, SIGNAL(startExperiment()), this, SLOT(onExperimentStart()));
    connect(experiSetting, SIGNAL(exitExperimentSetting()), this, SLOT(onExitExperiSetting()));
    connect(inExperiment, SIGNAL(pauseExperiment()), this, SLOT(onExperimentPaused()));
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
}

void MainWindow::on_btnExperiData_clicked()
{
    ui->stackedWidget->setCurrentIndex(experiDataIndex);
}

void MainWindow::on_btnExperiApp_clicked()
{
    ui->stackedWidget->setCurrentIndex(appSelcIndex);
}

void MainWindow::on_btnHelpDoc_clicked()
{
    ui->stackedWidget->setCurrentIndex(helpDocIndex);
}

void MainWindow::on_btnAOPI_clicked()
{
    ui->stackedWidget->setCurrentIndex(experiSettingIndex);
}

void MainWindow::on_btnDebugPage_clicked()
{
    ui->stackedWidget->setCurrentIndex(debugModeIndex);
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
    ui->stackedWidget->setCurrentIndex(inExperimentIndex);
    inExperiment->updateNoticeText("start experiment");
}

void MainWindow::onExperimentPaused()
{
    ui->stackedWidget->setCurrentIndex(appSelcIndex);
}

void MainWindow::onExitExperiSetting()
{
    ui->stackedWidget->setCurrentIndex(appSelcIndex);
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
