#include "exportDataDialog.h"
#include "ui_exportDataDialog.h"
#include <QProcess>
#include <QDateTime>
#include <QDir>
#include <QDebug>

QStringList exportDataType = {
    "ImageData",
    "ExcelData",
    "PDFData",
};

exportDataDialog::exportDataDialog(int type, QString experiID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exportDataDialog)
{
    ui->setupUi(this);

    dataType = exportDataType[type];

    ui->lblNoticeText->setText("Detecting Storage Device");

    QString upan;
    upan = detectUpan();
    if (upan.isEmpty()) {
        ui->lblNoticeText->setText("Error: Can not find storage device, Please retry");
        this->reject();
    } else {
        ui->lblNoticeText->setText("Found storage device: " + upan);
        if (createDir(upan, dataType)) {
            bool ret = true;
            switch(type) {
            case 0:
                ret = exportImageData(experiID);
                emit exportDataOk(ret);
                this->accept();
                break;
            case 1:
                break;
            case 2:
                break;
            deafult:
                break;
            }
        }
    }

}

exportDataDialog::~exportDataDialog()
{
    delete ui;
}

QString exportDataDialog::executeShellCmd(QString strCmd, int timeout)
{
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << strCmd);
    proc.waitForFinished(timeout);
    QString strRet = proc.readAllStandardOutput();
    return strRet;
}

QString exportDataDialog::detectUpan()
{
    QString upanName;
    int retryCount = 0;

    while (upanName.isEmpty()) {
        upanName = executeShellCmd("sleep 1 && ls /media/seekgene/", 5000);
        if (retryCount++ >= 10)
            break;
    }
    return upanName.trimmed();
}

bool exportDataDialog::createDir(QString &upan, QString &type)
{
    QString dirName = "seekgene_share_" + QDateTime::currentDateTime().toString("yyyyMMdd");
    QString path = "/media/seekgene/" + upan + "/"+ dirName;

    QDir dir(path);
    qDebug() << path;
    if (!dir.exists()) {
        if (dir.mkdir(path)) {
            qDebug() << "create dir " << path;
        } else {
            qDebug() << "create dir failed " << path;
            return false;
        }
    }
    dataSavePath = path + "/" + type;
    qDebug() << dataSavePath;
    QDir dirImage(dataSavePath);
    if (!dirImage.exists()) {
        if (dirImage.mkdir(path)) {
            qDebug() << "create dir " << dataSavePath;
        } else {
            qDebug() << "create dir failed " << dataSavePath;
            return false;
        }
    }
    ui->lblNoticeText->setText("Create directory ok, start to export data...");
    return true;
}

bool exportDataDialog::exportImageData(QString &experiID)
{
    ui->lblNoticeText->setText("Exporting data...");
    QString cmd = QString("cp -r /cellImages/%1/ %2/").arg(experiID).arg(dataSavePath);
    qDebug() << cmd;
    QString ret = executeShellCmd(cmd, 30000);
    if (ret.isEmpty()) {
        ui->lblNoticeText->setText("Export image data ok");
        return true;
    } else {
        ui->lblNoticeText->setText("Export image data failed, please retry.");
        return false;
    }

}
