#include "ExperiSetting.h"

ExperiSetting::ExperiSetting(QObject *parent) : QObject(parent)
{
    for (int i = 0; i < CHAMBER_NUM; i++) {
        m_chamberSelc[i] = 0;
    }
    m_chamberSet = 0;

}


