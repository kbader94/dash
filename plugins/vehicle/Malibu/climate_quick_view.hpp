#pragma once

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QFrame>
#include "app/quick_views/quick_view.hpp"
#include "app/utilities/icon_engine.hpp"
#include "app/widgets/step_meter.hpp"
#include "vent_mode.hpp"
#include "malibu.hpp"

class Arbiter;
class MalibuHVAC;

class ClimateControls : public QFrame, public QuickView {
    Q_OBJECT

   public:
    ClimateControls(Arbiter &arbiter);

    private:
    MalibuHVAC *HVAC;

    QHBoxLayout *horizontalLayout;

    QPushButton *driverSeatButton;
    QPushButton *autoButton;
    QPushButton *tempDownButton;
    QLabel *tempLabel;
    QPushButton *tempUpButton;
    QPushButton *passengerTempDownButton;
    QLabel *passengerTempLabel;
    QPushButton *passengerTempUpButton;
    QPushButton *passengerSeatButton;
    QPushButton *syncButton;

    QPushButton *acButton;
    QPushButton *defrostButton;
   // VentModeWidget *climateState;
    ClimateState *climateState;
    QPushButton *fanSpeedDownButton;
    StepMeter *fan_speed;
    QPushButton *fanSpeedUpButton;
    QPushButton *recircButton;

    StylizedIconEngine *ac_max_icon_engine;
    StylizedIconEngine *ac_eco_icon_engine;
    StylizedIconEngine *ac_off_icon_engine;
    StylizedIconEngine *auto_icon_engine_off;
    StylizedIconEngine *auto_icon_engine_on;
    StylizedIconEngine *sync_icon_engine_off;
    StylizedIconEngine *sync_icon_engine_on;
    StylizedIconEngine *seat_off_icon_engine;
    StylizedIconEngine *seat_1_icon_engine;
    StylizedIconEngine *seat_2_icon_engine;
    StylizedIconEngine *seat_3_icon_engine;
    StylizedIconEngine *defrost_icon_engine;

    QIcon ac_max_icon;
    QIcon ac_eco_icon;
    QIcon ac_off_icon;
    
    QIcon driver_auto_on_icon;
    QIcon driver_auto_off_icon;
    QIcon passenger_auto_on_icon;
    QIcon passenger_auto_off_icon;

    QIcon auto_on_icon;
    QIcon auto_off_icon;

    QIcon seat_off_icon;
    QIcon seat_1_icon;
    QIcon seat_2_icon;
    QIcon seat_3_icon;

    QIcon defrost_icon;

    void init() override;
};




