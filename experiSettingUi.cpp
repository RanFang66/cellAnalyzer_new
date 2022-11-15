#include "experiSettingUi.h"
#include "ui_experiSettingUi.h"
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <DlgAnimationUi.h>
experiSettingUi::experiSettingUi(ExperiSetting *setting, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::experiSettingUi),
    m_setting(setting)
{
    ui->setupUi(this);
    loadStyleSheet(":/styles/experiSettingStyle.qss");
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
    ui->comboCellType->addItem("human blood", CELL_HUMAN_BLOOD);
    ui->comboCellType->addItem("human bone", CELL_HUMAN_BONE);
    ui->comboCellType->addItem("mice liver", CELL_MICE_LIVER);

    ui->cBoxChannel1->setChecked(false);
    ui->cBoxChannel2->setChecked(false);
    ui->cBoxChannel3->setChecked(false);
    ui->cBoxChannel4->setChecked(false);
    ui->cBoxChannel5->setChecked(false);

    ui->editExperiName->setText(m_setting->experiName());
    ui->editExperiType->setText(m_setting->getExperiType());
    ui->editExperiUser->setText(m_setting->getUserName());
    ui->editSampleId->setText(m_setting->sampleId());
    ui->editCellSize->setText(QString("%1~%2 um").arg(m_setting->getCellMinRadiu()).arg(m_setting->getCellMaxRadiu()));
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
    DlgAnimationUi *animation = new DlgAnimationUi(this);
    animation->setAttribute(Qt::WA_DeleteOnClose);
    Qt::WindowFlags flags = animation->windowFlags();
    animation->setWindowFlags(flags | Qt::WindowStaysOnTopHint);

    if (animation->exec() == QDialog::Accepted) {
        emit startExperiment();
    }
}

void experiSettingUi::on_btnReturn_clicked()
{
    emit exitExperimentSetting();
}

void experiSettingUi::on_cBoxChannel1_clicked(bool checked)
{
    m_setting->setChamberEn(0, checked);
}

void experiSettingUi::on_cBoxChannel2_clicked(bool checked)
{
    m_setting->setChamberEn(1, checked);
}

void experiSettingUi::on_cBoxChannel3_clicked(bool checked)
{
    m_setting->setChamberEn(2, checked);
}

void experiSettingUi::on_cBoxChannel4_clicked(bool checked)
{
    m_setting->setChamberEn(3, checked);
}

void experiSettingUi::on_cBoxChannel5_clicked(bool checked)
{
    m_setting->setChamberEn(4, checked);
}

void experiSettingUi::on_editSampleId_textEdited(const QString &arg1)
{
    m_setting->setSampleId(arg1);
}

void experiSettingUi::on_comboCellType_currentIndexChanged(int index)
{
    m_setting->setCellTypeID(index);
    ui->editCellSize->setText(QString("%1~%2 um").arg(m_setting->getCellMinRadiu()).arg(m_setting->getCellMaxRadiu()));
}

void experiSettingUi::on_editExperiName_textEdited(const QString &arg1)
{
    m_setting->setExperiName(arg1);
}

void experiSettingUi::on_comboDilutionRate_currentIndexChanged(int index)
{
    m_setting->setDilutionRatio(index);
}
