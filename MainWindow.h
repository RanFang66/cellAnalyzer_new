#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <experiSettingUi.h>
#include <experiDataUi.h>
#include <debugModeUi.h>
#include <inExperimentUi.h>
#include <experiResultUi.h>

#include "DevCtrl.h"
#include "ExperiData.h"
#include "CellImageAlogrithm.h"
#include "ExperiCtrl.h"
#include "UserManageUi.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int *getUserIdPointer(void);

signals:
    void upanDetected(bool);

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

    void onExperimentStart();

    void onExperimentPaused();

    void onExitExperiSetting();

    void onExperimentFinished();

    void onShowDataDetail(QString &id);

    void onReturnToMainPage();

    void on_btnUserManage_clicked();

    void onReturnSysSetting();

    void on_btnLanguage_clicked();

    void on_btnShutdown_clicked();

    void on_btnWifiConfig_clicked();

private:
    Ui::MainWindow      *ui;
    experiSettingUi     *experiSetting;
    experiDataUi        *experiData;
    debugModeUi         *debugMode;
    inExperimentUi      *inExperiment;
    experiResultUi      *experiRes;
    UserManageUi        *userManage;

    DevCtrl             *m_dev;
    ExperiSetting       *m_setting;
    ExperiData          *m_data;
    ExperiCtrl          *m_experiCtrl;
    CellImageAlogrithm  *m_algorithm;

    QString m_userName;
    int m_userId;

    int appSelcIndex = 0;
    int sysSettingIndex = 1;
    int helpDocIndex = 2;
    int languageSelcIndex = 3;
    int experiSettingIndex;
    int experiDataIndex;
    int debugModeIndex;
    int inExperimentIndex;
    int experiResultIndex;
    int userManageIndex;

    int mainIndex;

    void initMainWindowUi();
    void loadStyleSheet(const QString &styleSheetFile);
    QString executeShellCmd(QString strCmd);
};

inline int *MainWindow::getUserIdPointer()
{
    return &m_userId;
}
#endif // MAINWINDOW_H
