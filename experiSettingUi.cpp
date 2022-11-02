#include "experiSettingUi.h"
#include "ui_experiSettingUi.h"
#include <QDateTime>
experiSettingUi::experiSettingUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::experiSettingUi)
{
    ui->setupUi(this);

    experiName = QDateTime::currentDateTime().toString();
    sampleId = "sample 1";
    userName = "admin";
    experiType = "AOPI viability";
    cellSize = "5~10(um)";

    initExperiSettingUi();
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
    ui->comboCellType->addItem("mice liver", CELL_MICE_LIVER);
    ui->comboCellType->addItem("human bone", CELL_HUMAN_BONE);

    ui->editExperiName->setText(experiName);
    ui->editExperiType->setText(experiType);
    ui->editExperiUser->setText(userName);
    ui->editSampleId->setText(sampleId);
    ui->editCellSize->setText(cellSize);
}
