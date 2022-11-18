#include "experiDataUi.h"
#include "ui_experiDataUi.h"
#include <QFile>
#include <QDebug>
#include <QMessageBox>
experiDataUi::experiDataUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::experiDataUi)
{
    ui->setupUi(this);
    loadStyleSheet(":/styles/experiDataStyle.qss");
    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);
    initExperiDataUi();
}

experiDataUi::~experiDataUi()
{
    delete ui;
}

void experiDataUi::updateExperiDataUi()
{
    qryModel->query().exec();
    theSelection->clear();
    ui->btnDeleteData->setEnabled(false);
    ui->btnDetail->setEnabled(false);
}

void experiDataUi::initExperiDataUi()
{
    ui->dateEditStart->setDate(QDate::currentDate().addDays(-1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    ui->comboType->clear();
    query->exec("SELECT cellTypeName from cellType");
    while (query->next()) {
        ui->comboType->addItem(query->value(0).toString());
    }

    ui->comboUser->clear();
    query->exec("SELECT name from userInfo");
    while (query->next()) {
        ui->comboUser->addItem(query->value(0).toString());
    }

    qryModel = new QSqlQueryModel(this);
    qryModel->setQuery("SELECT * FROM experiData ORDER BY endTime DESC", db);
    if (qryModel->lastError().isValid()) {
        QMessageBox::critical(this, "Error", "Query Data Failed\n"+qryModel->lastError().text());
        return;
    }

    qryModel->setHeaderData(0, Qt::Horizontal, "ID");
    qryModel->setHeaderData(1, Qt::Horizontal, "Name");
    qryModel->setHeaderData(2, Qt::Horizontal, "User");
    qryModel->setHeaderData(3, Qt::Horizontal, "Type");
    qryModel->setHeaderData(4, Qt::Horizontal, "Cell Type");
    qryModel->setHeaderData(5, Qt::Horizontal, "Chamber");
    qryModel->setHeaderData(6, Qt::Horizontal, "Sample ID");
    qryModel->setHeaderData(7, Qt::Horizontal, "Dilution Ratio");
    qryModel->setHeaderData(8, Qt::Horizontal, "Cell Conc");
    qryModel->setHeaderData(9, Qt::Horizontal, "Live Cell Conc");
    qryModel->setHeaderData(10, Qt::Horizontal, "Dead Cell Conc");
    qryModel->setHeaderData(11, Qt::Horizontal, "Viability");
    qryModel->setHeaderData(12, Qt::Horizontal, "Cell Number");
    qryModel->setHeaderData(13, Qt::Horizontal, "Live Cell Number");
    qryModel->setHeaderData(14, Qt::Horizontal, "Dead Cell Number");
    qryModel->setHeaderData(15, Qt::Horizontal, "Average Diameter");
    qryModel->setHeaderData(16, Qt::Horizontal, "Average Compactness");
    qryModel->setHeaderData(17, Qt::Horizontal, "Aggregate Rate");
    qryModel->setHeaderData(18, Qt::Horizontal, "Time");

    theSelection = new QItemSelectionModel(qryModel);
    connect(theSelection, SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(onCurrentRowChanged(QModelIndex, QModelIndex)));
    ui->tblExperiData->setModel(qryModel);
    ui->tblExperiData->setSelectionModel(theSelection);
    ui->tblExperiData->verticalHeader()->setVisible(false);
    ui->tblExperiData->setColumnHidden(0, true);
    theSelection->clear();
    ui->btnDeleteData->setEnabled(false);
    ui->btnDetail->setEnabled(false);

}

void experiDataUi::loadStyleSheet(const QString &styleSheetFile)
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

QString experiDataUi::executeShellCmd(QString strCmd, int timeout)
{
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << strCmd);
    proc.waitForFinished(timeout);
    QString strRet = proc.readAllStandardOutput();
    return strRet;
}

QString experiDataUi::detectUpan()
{
    QString upanName;
    int retryCount = 0;
    QString ret = executeShellCmd("sudo raspi-gpio set 26 op dl", 5000);
    while (upanName.isEmpty()) {
        upanName = executeShellCmd("sleep 1 && ls /media/seekgene/", 5000);
        if (retryCount++ >= 10)
            break;
    }
    return upanName.trimmed();
}


void experiDataUi::on_btnDeleteData_clicked()
{
//    int curRecNo = theSelection->currentIndex().row();
    if (curRec.isEmpty()) {
        return;
    }


    QString warningStr = QString("Delete the data of Experiment_%1")+ (curRec.value("experiID").toString());
    QMessageBox::StandardButton res = QMessageBox::question(this, "Delete Record", warningStr);
    if (res == QMessageBox::Yes) {
        QSqlQuery query(db);
        query.prepare("DELETE FROM experiData WHERE experiID = :ID");
        query.bindValue(":ID", curExperiID);
        if (!query.exec()) {
            QMessageBox::critical(this, "Error", "Delete record failed!" + query.lastError().text());
        } else {
            qryModel->query().exec();
        }
    } else {
        return;
    }
}

void experiDataUi::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    ui->btnDeleteData->setEnabled(current.isValid());
    ui->btnDetail->setEnabled(current.isValid());
    if (current.isValid()) {
        curRec = qryModel->record(current.row());
        curExperiID = curRec.value("experiID").toString();
    }
}

void experiDataUi::on_btnDetail_clicked()
{
    emit showDataDetail(curExperiID);
}


void experiDataUi::on_btnPDF_clicked()
{
    QString upan = detectUpan();
    if (upan.isEmpty()) {
        QMessageBox::critical(this, "Error", "Can not find upan, please retry");
        return;
    }
    QString dirName = "seekgene_share_" + QDateTime::currentDateTime().toString("yyyyMMdd");
    QString path = "/media/seekgene/" + upan +"/"+ dirName;

    QDir dir(path);
    if (!dir.exists()) {
        dir.mkdir(path);
    }
    QString pathPDF = path + "/pdfData";
    QDir dirPdf(pathPDF);
    if (!dirPdf.exists()) {
        dirPdf.mkdir(pathPDF);
    }


    m_exportType = EXPORT_PDF;
}

void experiDataUi::on_btnExcel_clicked()
{
    QString upan = detectUpan();
    if (upan.isEmpty()) {
        QMessageBox::critical(this, "Error", "Can not find upan, please retry");
        return;
    }
    QString dirName = "seekgene_share_" + QDateTime::currentDateTime().toString("yyyyMMdd");
    QString path = "/media/seekgene/" + upan +"/"+ dirName;

    QDir dir(path);
    if (!dir.exists()) {
        dir.mkdir(path);
    }
    QString pathCSV = path + "/csvData";
    QDir dirCSV(pathCSV);
    if (!dirCSV.exists()) {
        dirCSV.mkdir(pathCSV);
    }
    m_exportType = EXPORT_CSV;
}

void experiDataUi::on_btnJPG_clicked()
{
    QString upan = detectUpan();
    if (upan.isEmpty()) {
        QMessageBox::critical(this, "Error", "Can not find upan, please retry");
        return;
    }
    QString dirName = "seekgene_share_" + QDateTime::currentDateTime().toString("yyyyMMdd");
    QString path = "/media/seekgene/" + upan +"/"+ dirName;

    QDir dir(path);
    qDebug() << path;
    if (!dir.exists()) {
        if (dir.mkdir(path)) {
            qDebug() << "create dir " << path;
        } else {
            qDebug() << "create dir failed " << path;
        }
    }
    QString pathImage = path + "/imageData";
    qDebug() << pathImage;
    QDir dirImage(pathImage);
    if (!dirImage.exists()) {
        if (dirImage.mkdir(path)) {
            qDebug() << "create dir " << pathImage;
        } else {
            qDebug() << "create dir failed " << pathImage;
        }
    }

    QString cmd = QString("cp -r /cellImages/%1/ %2/").arg(curExperiID).arg(pathImage);
    qDebug() << cmd;
    QString ret = executeShellCmd(cmd, 30000);
    //qDebug() << ret;
    if (ret.isEmpty()) {
        QMessageBox::information(this, "Export Ok", "Export image data ok!");
    }
    m_exportType = EXPORT_IMAGE;
}
