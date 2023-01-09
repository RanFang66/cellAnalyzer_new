#include "wificonfigui.h"
#include "ui_wificonfigui.h"
#include <QNetworkConfigurationManager>
#include <QFile>
#include <QDebug>
#include <QGridLayout>
#include <QMouseEvent>
#include <inputpassworddialog.h>
#include <QNetworkConfiguration>

WifiConfigUi::WifiConfigUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WifiConfigUi)
{
    loadStyleSheet(":/styles/wifiConfig.qss");
    ui->setupUi(this);
    curSelcLine = 0;
    curConnectedLine = 0;

//    initWifiList();
    connect(this, SIGNAL(refreshWifi()), this, SLOT(initWifiList()), Qt::QueuedConnection);
    emit refreshWifi();
}

WifiConfigUi::~WifiConfigUi()
{
    delete ui;
}

void WifiConfigUi::initWifiList()
{
    system("nmcli device wifi > /home/ran/wifi.ini");
    QFile file("/home/ran/wifi.ini");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "No such file";
        return;
    }

    QVBoxLayout *vLayout = new QVBoxLayout;
    int curLine = 0;
    QString line;
    if (file.size() != 0) {
        QTextStream in(&file);
        line = in.readLine();
        if (!line.isEmpty()){
            int signalIndex = line.indexOf("SIGNAL");
            int wifiIndex = line.indexOf(" SSID") + 1;
            int modeIndex = line.indexOf("MODE");
            while (curLine < WIFI_LIST_LEN) {
                line = in.readLine();
                QString tmpStr = line;
                QString curConnection = line.left(1);
                QString wifiName = line.mid(wifiIndex, (modeIndex-wifiIndex)).simplified();
                int signalValue = tmpStr.mid(signalIndex, 3).simplified().toInt();
                if(!wifiName.isEmpty()){
                    QGridLayout *gridLayout = new QGridLayout();
                    wifiWidget[curLine] = new QWidget(this);
                    wifiWidget[curLine]->setObjectName(QString("wifiName*%1*%2").arg(curLine).arg(wifiName));
                    wifiWidget[curLine]->installEventFilter(this);
//                    wifiWidget[curLine]->setStyleSheet(QString("QWidget::hover{background-color:darkgray}").append(QString("QWidget{background-color:transparent; color: rgba(255, 255, 255, 0)}")));
//                    wifiWidget[curLine]->setStyleSheet(QString("QWidget{font-size: 3font-weight: 400;color: rgba(255, 255, 255, 1);}"));
                    wifiNameLabel[curLine] = new QLabel(wifiWidget[curLine]);
                    wifiNameLabel[curLine]->setText(wifiName);

                    iconLabel[curLine] = new QLabel(wifiWidget[curLine]);
                    iconLabel[curLine]->setText(QString::number(signalValue));

                    connectLabel[curLine] = new QLabel(wifiWidget[curLine]);
                    if (curConnection == "*") {
                        connectLabel[curLine]->setText("Connected");
                        curConnectedLine = curLine;
                    }
                    connectButton[curLine] = new QPushButton(wifiWidget[curLine]);
//                    connectButton[curLine]->setStyleSheet(QString("QPushButton{background-color:lightgray;}").append(QString("QPushButton::pressed{background-color:darkgray;}")));
                    connectButton[curLine]->setText("Connect");
                    connectButton[curLine]->setVisible(false);
                    gridLayout->addWidget(wifiNameLabel[curLine], 0, 0, 1, 3);
                    gridLayout->addWidget(connectLabel[curLine], 0, 3, 1, 1);
                    gridLayout->addWidget(iconLabel[curLine], 0, 4, 1, 1);
                    gridLayout->addWidget(connectButton[curLine], 1, 3, 1, 1);

                    wifiWidget[curLine]->setLayout(gridLayout);
                    vLayout->addWidget(wifiWidget[curLine]);
                    vLayout->setMargin(5);
                    vLayout->setSpacing(2);
                }
                curLine++;
            }
        }
    }
    file.close();
    ui->groupWifiList->setLayout(vLayout);
}

void WifiConfigUi::refreshWifiList()
{
    system("nmcli device wifi > /home/ran/wifi.ini");
    QFile file("/home/ran/wifi.ini");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "No such file";
        return;
    }

    int curLine = 0;
    QString line;
    if (file.size() != 0) {
        QTextStream in(&file);
        line = in.readLine();
        if (!line.isEmpty()){
            int signalIndex = line.indexOf("SIGNAL");
            int wifiIndex = line.indexOf(" SSID") + 1;
            int modeIndex = line.indexOf("MODE");
            while (curLine < WIFI_LIST_LEN) {
                line = in.readLine();
                QString tmpStr = line;
                QString curConnection = line.left(1);
                QString wifiName = line.mid(wifiIndex, (modeIndex-wifiIndex)).simplified();
                int signalValue = tmpStr.mid(signalIndex, 3).simplified().toInt();
                if (!wifiName.isEmpty()) {
                    wifiWidget[curLine]->setObjectName(QString("wifiName*%1*%2").arg(curLine).arg(wifiName));
                    wifiWidget[curLine]->installEventFilter(this);
//                    wifiWidget[curLine]->setStyleSheet(QString("QWidget{font-size: 3font-weight: 400;color: rgba(255, 255, 255, 1);}"));

//                    wifiWidget[curLine]->setStyleSheet(QString("QWidget::hover{background-color:darkgray}").append(QString("QWidget{background-color:transparent; color: rgba(255, 255, 255, 0)}")));

                    wifiNameLabel[curLine]->setText(wifiName);

                    iconLabel[curLine]->setText(QString::number(signalValue));
                    if (curConnection == "*") {
                        connectLabel[curLine]->setText("已连接");
                        curConnectedLine = curLine;
                    }

//                    connectButton[curLine]->setStyleSheet(QString("QPushButton{background-color:lightgray;}").append(QString("QPushButton::pressed{background-color:darkgray;}")));
                    connectButton[curLine]->setText("连接");
                    connectButton[curLine]->setVisible(false);
                }
                curLine++;
            }
        }
    }
    file.close();
}

void WifiConfigUi::on_btnReturn_clicked()
{
    emit return2SysSetting();
}

bool WifiConfigUi::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if(mouseEvent->type() == QEvent::MouseButtonPress){
        if(mouseEvent->button()==Qt::LeftButton){
            if(obj->objectName().contains("wifiName")){
                QString wifiName = obj->objectName().section('*',-1);
                connectButton[curSelcLine]->setVisible(false);                  //隐藏所有wifi连接button
                curSelcLine = obj->objectName().section('*',1,1).toInt();
                connectButton[curSelcLine]->setVisible(true);                   //选中当前wifi属性框后，显示wifi连接按钮

                connect(connectButton[curSelcLine],&QPushButton::clicked,[=](){
                    InputPasswordDialog *inputPassword = new InputPasswordDialog();     //实例化wifi密码输入窗口

                    inputPassword->show();
                    connect(inputPassword,&InputPasswordDialog::confirmPassword,[=](QString password){
                        QString connectNetWorkStr = QString("nmcli dev wifi connect %1 password %2");
                        QByteArray array = connectNetWorkStr.arg(wifiName).arg(password).toLatin1();
                        char *cmdStr = array.data();        //QString 转 char*
                        if(-1 != system(cmdStr)){
                            inputPassword->hide();
                            QNetworkConfigurationManager mgr;
                            if(mgr.isOnline()){             //wifi已连接
                                qDebug("wifi is online!");
                                connectLabel[curSelcLine]->setText(tr("已连接"));
                                connectLabel[curConnectedLine]->setText("");
                                curConnectedLine = curSelcLine;
                                connectButton[curSelcLine]->setVisible(false);
                            }
                            else{
                                qDebug("wifi is offline!");
                                connectLabel[curConnectedLine]->setText("");
                                connectButton[curSelcLine]->setVisible(false);
                            }
                        }
                    });
                });
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void WifiConfigUi::loadStyleSheet(const QString &styleSheetFile)
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


void WifiConfigUi::on_btnRefresh_clicked()
{
    refreshWifiList();

}

