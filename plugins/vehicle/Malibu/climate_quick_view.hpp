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
#include "app/widgets/step_meter.hpp"
#include "app/widgets/climate_state.hpp"
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
    ClimateState *climateState;
    QPushButton *fanSpeedDownButton;
    StepMeter *fan_speed;
    QPushButton *fanSpeedUpButton;
    QPushButton *recircButton;

    void setTemp(double temp);
    void setFanSpeed(int speed);
    //void setAirflow(Airflow airflow);
    void setDefrost(bool defrost);

    void init() override;
};




