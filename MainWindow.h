#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <experiSettingUi.h>
#include <experiDataUi.h>
#include <debugModeUi.h>
#include <QSerialWorker.h>
#include <SerialProtocol.h>
#include <CameraCtrl.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnSysSetting_clicked();

    void on_btnExperiData_clicked();

    void on_btnExperiApp_clicked();

    void on_btnHelpDoc_clicked();

    void on_btnAOPI_clicked();

    void on_btnDebugPage_clicked();

    void onSerialConnected(bool);
    void onSerialDisconnected();

    void onCameraInitRet(bool);
private:
    Ui::MainWindow      *ui;
    experiSettingUi     *experiSetting;
    experiDataUi        *experiData;
    debugModeUi         *debugMode;

    QSerialWorker       *m_serialWorker;
    QThread             *m_serialThread;
    SerialProtocol      *m_serialProtocol;

    CameraCtrl          *m_camCtrl;
    QThread             *m_camThread;
    int appSelcIndex = 0;
    int sysSettingIndex = 1;
    int helpDocIndex = 2;
    int experiSettingIndex;
    int experiDataIndex;
    int debugModeIndex;

    void initMainWindowUi();
    void initSerialComm();
    void initCameraCtrl();
};
#endif // MAINWINDOW_H
