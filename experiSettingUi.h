#ifndef EXPERISETTINGUI_H
#define EXPERISETTINGUI_H

#include <QWidget>
#include <ExperiSetting.h>
#include <DlgAnimationUi.h>

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

    enum CHAMBER_ID {
        CHAMBER1 = 1,
        CHAMBER2,
        CHAMBER3,
        CHAMBER4,
        CHAMBER5,
        CHAMBER6,
    };

    explicit experiSettingUi(ExperiSetting *setting, QWidget *parent = nullptr);
    ~experiSettingUi();
    void initExperiSettingUi();

public slots:
    void setChipState(int state);

private slots:
    void on_btnNextStep_clicked();

    void on_btnReturn_clicked();

    void on_editSampleId_textEdited(const QString &arg1);

    void on_editExperiName_textEdited(const QString &arg1);


    void updateSettingInfo(void);

    void on_btnConfirmSetting_clicked();


    void on_cBoxChannel5_clicked();

    void on_cBoxChannel1_clicked();

    void on_cBoxChannel2_clicked();

    void on_cBoxChannel3_clicked();

    void on_cBoxChannel4_clicked();

    void on_cBoxChannel6_clicked();

    void on_comboCellType_activated(int index);

signals:
    void startExperiment();
    void exitExperimentSetting();

private:
    Ui::experiSettingUi *ui;
    ExperiSetting       *m_setting;
    DlgAnimationUi      *animation;
    bool m_chipState;
    QSqlDatabase db;
    QSqlQuery *query;

    int     currentChamber;
    void loadStyleSheet(const QString &styleSheetFile);
};

inline void experiSettingUi::setChipState(int state)
{
    m_chipState = state;
    if (animation) {
        animation->setChipState(state);
    }
}

#endif // EXPERISETTINGUI_H
