#ifndef EXPERISETTINGUI_H
#define EXPERISETTINGUI_H

#include <QWidget>
#include <ExperiSetting.h>

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

    explicit experiSettingUi(QWidget *parent = nullptr);
    ~experiSettingUi();

private slots:
    void on_btnNextStep_clicked();

    void on_btnReturn_clicked();

signals:
    void startExperiment();
    void exitExperimentSetting();

private:
    Ui::experiSettingUi *ui;
    ExperiSetting       *setting;
    void initExperiSettingUi();
    void loadStyleSheet(const QString &styleSheetFile);
};

#endif // EXPERISETTINGUI_H
