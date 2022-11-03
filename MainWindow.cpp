#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString ret = executeShellCmd("sudo raspi-gpio set 26 op dh");
    qDebug() << ret;
    m_dev = new DevCtrl(this);

    experiSetting = new experiSettingUi(this);
    experiData = new experiDataUi(this);
    debugMode = new debugModeUi(m_dev, this);

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

    ui->stackedWidget->setCurrentIndex(appSelcIndex);
}

QString MainWindow::executeShellCmd(QString strCmd)
{
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << strCmd);
    proc.waitForFinished(1000);
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
