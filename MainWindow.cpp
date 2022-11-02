#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    experiSetting = new experiSettingUi(this);
    experiData = new experiDataUi(this);
    debugMode = new debugModeUi(this);


    initMainWindowUi();
    initSerialComm();
    initCameraCtrl();
}

MainWindow::~MainWindow()
{
    if(m_serialThread) {
        m_serialThread->quit();
        m_serialThread->wait();
        m_serialThread->deleteLater();
    }
    delete m_serialWorker;
    delete m_serialProtocol;
    delete ui;
}

void MainWindow::initMainWindowUi()
{
    experiSettingIndex = ui->stackedWidget->addWidget(experiSetting);
    experiDataIndex = ui->stackedWidget->addWidget(experiData);
    debugModeIndex = ui->stackedWidget->addWidget(debugMode);

    ui->stackedWidget->setCurrentIndex(appSelcIndex);
}

void MainWindow::initSerialComm()
{
    m_serialProtocol = new SerialProtocol();
    m_serialWorker = new QSerialWorker();
    m_serialThread = new QThread();
    m_serialWorker->moveToThread(m_serialThread);

    QList<QSerialPortInfo> serInfo = QSerialPortInfo::availablePorts();
    if (serInfo.length() > 0) {
        m_serialWorker->setSerialParams(serInfo[0].portName());
    }
    connect(m_serialWorker, &QSerialWorker::serialRecvData, m_serialProtocol, &SerialProtocol::onSerialRecvData);
    connect(m_serialProtocol, &SerialProtocol::writeToSerial, m_serialWorker, &QSerialWorker::sendSerialData);
    connect(m_serialWorker, &QSerialWorker::serialConnected, this, &MainWindow::onSerialConnected);
    connect(m_serialWorker, &QSerialWorker::serialDisconnected, this, &MainWindow::onSerialDisconnected);
    connect(m_serialThread, &QThread::finished, m_serialWorker, &QObject::deleteLater);
    connect(m_serialThread, &QThread::started, m_serialWorker, &QSerialWorker::serialInit);
    m_serialThread->start();
}

void MainWindow::initCameraCtrl()
{
    m_camCtrl = new CameraCtrl();
    m_camThread = new QThread();
    m_camCtrl->moveToThread(m_camThread);
    connect(m_camCtrl, &CameraCtrl::cameraInitRet, this, &MainWindow::onCameraInitRet);
    connect(m_camCtrl, &CameraCtrl::imageUpdated, debugMode, &debugModeUi::onCamImageUpdated);
    connect(m_camThread, &QThread::finished, m_camCtrl, &QObject::deleteLater);
    connect(m_camThread, &QThread::started, m_camCtrl, &CameraCtrl::cameraInit);
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
