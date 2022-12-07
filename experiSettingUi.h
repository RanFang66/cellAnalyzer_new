#ifndef EXPERISETTINGUI_H
#define EXPERISETTINGUI_H

#include <QWidget>
#include <ExperiSetting.h>

const QString EXPERI_TYPE[] = {
    "AOPI Viability",
    "TAIPAN BLUE",
};

namespace Ui {
class experiSettingUi;
}

class experiSettingUi : public QWidget
{
    Q_OBJECT

public:
    enum DILUTION_RATIO {
        RATIO_1_1 = 1,
        RATIO_1_2,
        RATIO_1_3,
        RATIO_1_4,
        RATIO_1_5,
        RATIO_1_6,
        RATIO_1_7,
        RATIO_1_8,
        RATIO_1_9,
        RATIO_1_10,
    };
    enum CELL_TYPE {
        CELL_HUMAN_BLOOD,
        CELL_MICE_LIVER,
        CELL_HUMAN_BONE,
    };

    explicit experiSettingUi(ExperiSetting *setting, QWidget *parent = nullptr);
    ~experiSettingUi();
    void initExperiSettingUi();
private slots:
    void on_btnNextStep_clicked();

    void on_btnReturn_clicked();

    void on_cBoxChannel1_clicked(bool checked);

    void on_cBoxChannel2_clicked(bool checked);

    void on_cBoxChannel3_clicked(bool checked);

    void on_cBoxChannel4_clicked(bool checked);

    void on_cBoxChannel5_clicked(bool checked);

    void on_editSampleId_textEdited(const QString &arg1);

    void on_comboCellType_currentIndexChanged(int index);

    void on_editExperiName_textEdited(const QString &arg1);

    void on_comboDilutionRate_currentIndexChanged(int index);
signals:
    void startExperiment();
    void exitExperimentSetting();

private:
    Ui::experiSettingUi *ui;
    ExperiSetting       *m_setting;
    QSqlDatabase db;
    QSqlQuery *query;

    void loadStyleSheet(const QString &styleSheetFile);
};

#endif // EXPERISETTINGUI_H
