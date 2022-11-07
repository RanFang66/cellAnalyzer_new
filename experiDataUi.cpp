#include "experiDataUi.h"
#include "ui_experiDataUi.h"
#include <QFile>
#include <QDebug>
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

void experiDataUi::initExperiDataUi()
{
    ui->dateEditStart->setDate(QDate::currentDate().addDays(-1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    ui->comboType->clear();
    ui->comboType->addItem("AOPI cell vality");
    ui->comboType->addItem("Human blood");

    ui->comboUser->clear();
    query->exec("SELECT name from userInfo");
    while (query->next()) {
        ui->comboUser->addItem(query->value(0).toString());
    }

    tblModel = new QSqlTableModel(this, db);
    tblModel->setTable("experiData");
    tblModel->setSort(tblModel->fieldIndex("startTime"), Qt::DescendingOrder);
    tblModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    if (!(tblModel->select())) {
  //      QMessageBox::critical(this, "Error", "Open data table failed, error info: \n" + tblModel->lastError().text(), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    tblModel->setHeaderData(tblModel->fieldIndex("experiName"), Qt::Horizontal, "Experiment Name");
    tblModel->setHeaderData(tblModel->fieldIndex("startTime"), Qt::Horizontal, "Experiment Time");
    tblModel->setHeaderData(tblModel->fieldIndex("sampleId"), Qt::Horizontal, "Sample ID");
    tblModel->setHeaderData(tblModel->fieldIndex("userId"), Qt::Horizontal, "User Name");
    tblModel->setHeaderData(tblModel->fieldIndex("cellType"), Qt::Horizontal, "Cell Type");
    tblModel->setHeaderData(tblModel->fieldIndex("viability"), Qt::Horizontal, "Viability");
    tblModel->setHeaderData(tblModel->fieldIndex("cellConc"), Qt::Horizontal, "Cell Concentration");
    tblModel->setHeaderData(tblModel->fieldIndex("liveCellConc"), Qt::Horizontal, "Live Cell Concentration");
    tblModel->setHeaderData(tblModel->fieldIndex("deadCellConc"), Qt::Horizontal, "Dead Cell Concentration");
    tblModel->setHeaderData(tblModel->fieldIndex("totalCellNum"), Qt::Horizontal, "Total Cell Number");
    tblModel->setHeaderData(tblModel->fieldIndex("liveCellNum"), Qt::Horizontal, "Live Cell Number");
    tblModel->setHeaderData(tblModel->fieldIndex("deadCellNum"), Qt::Horizontal, "Dead Cell Number");
    tblModel->setHeaderData(tblModel->fieldIndex("avgDiameter"), Qt::Horizontal, "Average Diameter");
    tblModel->setHeaderData(tblModel->fieldIndex("avgCompactness"), Qt::Horizontal, "Average Compactness");
    tblModel->setHeaderData(tblModel->fieldIndex("aggregateRate"), Qt::Horizontal, "Aggregate Rate");

    theSelection = new QItemSelectionModel(tblModel);
    ui->tblExperiData->setModel(tblModel);
    ui->tblExperiData->setSelectionModel(theSelection);
    ui->tblExperiData->verticalHeader()->setVisible(false);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("experiId"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgBright_1"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgFLG_1"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgFLR_1"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgBright_2"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgFLG_2"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgFLR_2"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgBright_3"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgFLG_3"), true);
    ui->tblExperiData->setColumnHidden(tblModel->fieldIndex("imgFLR_3"), true);
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

