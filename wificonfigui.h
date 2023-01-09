#ifndef WIFICONFIGUI_H
#define WIFICONFIGUI_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

namespace Ui {
class WifiConfigUi;
}

#define WIFI_LIST_LEN (10)

class WifiConfigUi : public QWidget
{
    Q_OBJECT

public:
    explicit WifiConfigUi(QWidget *parent = nullptr);
    ~WifiConfigUi();



signals:
    void refreshWifi();
    void return2SysSetting();

private slots:
    void on_btnReturn_clicked();
    void initWifiList();
    void refreshWifiList();
    void on_btnRefresh_clicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::WifiConfigUi *ui;


    QLabel *iconLabel[WIFI_LIST_LEN];                 //显示wifi信号强度
    QLabel *connectLabel[WIFI_LIST_LEN];              //显示wifi连接状态
    QLabel *wifiNameLabel[WIFI_LIST_LEN];             //wifi名
    QWidget *wifiWidget[WIFI_LIST_LEN];               //包含wifi成员的widget
    QPushButton *connectButton[WIFI_LIST_LEN];        //wifi连接按钮
    int     curSelcLine;
    int     curConnectedLine;

    void loadStyleSheet(const QString &styleSheetFile);
};

#endif // WIFICONFIGUI_H
