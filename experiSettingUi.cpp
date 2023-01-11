#include "experiSettingUi.h"
#include "ui_experiSettingUi.h"
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

experiSettingUi::experiSettingUi(ExperiSetting *setting, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::experiSettingUi),
    m_setting(setting)
{
    ui->setupUi(this);
    loadStyleSheet(":/styles/experiSettingStyle.qss");

    db = QSqlDatabase::database("cellDataConn");
    query = new QSqlQuery(db);
    currentChamber = CHAMBER1;
//    initExperiSettingUi();
}

experiSettingUi::~experiSettingUi()
{
    delete ui;
}

void experiSettingUi::initExperiSettingUi()
{
    ui->comboDilutionRate->clear();
    ui->comboDilutionRate->addItem("1:1", RATIO_1_1);
    ui->comboDilutionRate->addItem("1:2", RATIO_1_2);
    ui->comboDilutionRate->addItem("1:3", RATIO_1_3);
    ui->comboDilutionRate->addItem("1:4", RATIO_1_4);
    ui->comboDilutionRate->addItem("1:5", RATIO_1_5);
    ui->comboDilutionRate->addItem("1:6", RATIO_1_6);
    ui->comboDilutionRate->addItem("1:7", RATIO_1_7);
    ui->comboDilutionRate->addItem("1:8", RATIO_1_8);
    ui->comboDilutionRate->addItem("1:9", RATIO_1_9);
    ui->comboDilutionRate->addItem("1:10", RATIO_1_10);


    ui->comboCellType->clear();
    query->exec("SELECT cellTypeName from cellType");
    while (query->next()) {
        ui->comboCellType->addItem(query->value(0).toString());
    }

    ui->cBoxChannel1->setChecked(false);
    ui->cBoxChannel2->setChecked(false);
    ui->cBoxChannel3->setChecked(false);
    ui->cBoxChannel4->setChecked(false);
    ui->cBoxChannel5->setChecked(false);
    ui->cBoxChannel6->setChecked(false);
    ui->cBoxChannel6->setEnabled(false);
    ui->editExperiName->setText(m_setting->experiName(CHAMBER1));
    ui->editExperiType->setText(m_setting->getExperiType());
    ui->editExperiUser->setText(m_setting->getUserName());
//    ui->editSampleId->setText(m_setting->sampleId(CHAMBER1));
    ui->editCellSize->setText(QString("%1~%2 um").arg(m_setting->getCellMinRadiu(CHAMBER1)).arg(m_setting->getCellMaxRadiu(CHAMBER1)));
}


void experiSettingUi::loadStyleSheet(const QString &styleSheetFile)
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

void experiSettingUi::on_btnNextStep_clicked()
{
    if (m_chipState == 1 &&  m_setting->chamberSet() != 0) {
         emit startExperiment();
    } else if (m_chipState != 1){
        animation = new DlgAnimationUi(this);
        animation->setAttribute(Qt::WA_DeleteOnClose);
        Qt::WindowFlags flags = animation->windowFlags();
        animation->setWindowFlags(flags | Qt::WindowStaysOnTopHint);

        if (animation->exec() == QDialog::Accepted) {
            emit startExperiment();
        } else {
            return;
        }
    } else if (m_setting->chamberSet() == 0) {
        QMessageBox::warning(this, "warning", "please select a chamber!");
        return;
    }
}

void experiSettingUi::on_btnReturn_clicked()
{
    emit exitExperimentSetting();
}


void experiSettingUi::on_editSampleId_textEdited(const QString &arg1)
{
    m_setting->setSampleId(currentChamber, arg1);
}


void experiSettingUi::on_editExperiName_textEdited(const QString &arg1)
{
    m_setting->setExperiName(currentChamber, arg1);
}


void experiSettingUi::updateSettingInfo()
{
    ui->editExperiName->setText(m_setting->experiName(currentChamber));
    ui->editSampleId->setText(m_setting->sampleId(currentChamber));
    ui->comboCellType->setCurrentIndex(m_setting->cellTypeID(currentChamber)-1);
    ui->comboDilutionRate->setCurrentIndex(m_setting->dilutionRatio(currentChamber)-1);
    ui->editCellSize->setText(QString("%1~%2 um").arg(m_setting->getCellMinRadiu(currentChamber)).arg(m_setting->getCellMaxRadiu(currentChamber)));
}

void experiSettingUi::on_btnConfirmSetting_clicked()
{
    QCheckBox *cbox[CHAMBER_NUM];

    cbox[0] = ui->cBoxChannel1;
    cbox[1] = ui->cBoxChannel2;
    cbox[2] = ui->cBoxChannel3;
    cbox[3] = ui->cBoxChannel4;
    cbox[4] = ui->cBoxChannel5;
    cbox[5] = ui->cBoxChannel6;

    for (int i = 0; i < CHAMBER_NUM; i++) {
        int chamberid = i+1;
        if (cbox[i]->checkState() == Qt::PartiallyChecked) {
            m_setting->setExperiName(chamberid, ui->editExperiName->text());
            QString sampleID = ui->editSampleId->text();
            if (!sampleID.isEmpty())
                m_setting->setSampleId(chamberid, ui->editSampleId->text());
            else{
                m_setting->setSampleId(chamberid, QString("Chamber%1").arg(chamberid));
            }
            m_setting->setCellTypeID(chamberid, ui->comboCellType->currentIndex() + 1);
            m_setting->setDilutionRatio(chamberid, ui->comboDilutionRate->currentIndex() + 1);
            m_setting->setChamberEn(chamberid, true);
            cbox[i]->setCheckState(Qt::Checked);
        } else if (cbox[i]->checkState() == Qt::Unchecked) {
            m_setting->setChamberEn(chamberid, false);
        }
    }
}


void experiSettingUi::on_cBoxChannel1_clicked()
{
    if (ui->cBoxChannel1->checkState() == Qt::Checked) {
        ui->cBoxChannel1->setCheckState(Qt::Unchecked);
    }
}

void experiSettingUi::on_cBoxChannel2_clicked()
{
    if (ui->cBoxChannel2->checkState() == Qt::Checked) {
        ui->cBoxChannel2->setCheckState(Qt::Unchecked);
    }
}

void experiSettingUi::on_cBoxChannel3_clicked()
{
    if (ui->cBoxChannel3->checkState() == Qt::Checked) {
        ui->cBoxChannel3->setCheckState(Qt::Unchecked);
    }
}

void experiSettingUi::on_cBoxChannel4_clicked()
{
    if (ui->cBoxChannel4->checkState() == Qt::Checked) {
        ui->cBoxChannel4->setCheckState(Qt::Unchecked);
    }
}

void experiSettingUi::on_cBoxChannel5_clicked()
{
    if (ui->cBoxChannel5->checkState() == Qt::Checked) {
        ui->cBoxChannel5->setCheckState(Qt::Unchecked);
    }
}

void experiSettingUi::on_cBoxChannel6_clicked()
{
    if (ui->cBoxChannel6->checkState() == Qt::Checked) {
        ui->cBoxChannel6->setCheckState(Qt::Unchecked);
    }
}




void experiSettingUi::on_comboCellType_activated(int index)
{
    QString queryStr = QString("SELECT * FROM cellType WHERE cellTypeID = %1").arg(index+1);
    int min, max;
    if (query->exec(queryStr) && query->next()) {
        min = query->value(2).toInt();
        max = query->value(3).toInt();
        ui->editCellSize->setText(QString("%1~%2 um").arg(min).arg(max));
    }
}

