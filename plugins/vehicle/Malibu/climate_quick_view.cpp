#include <QFrame>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QDebug>
#include "climate_dialog.h"

#include "app/arbiter.hpp"
#include "app/widgets/dialog.hpp"
#include "climate_quick_view.hpp"

ClimateControls::ClimateControls(Arbiter &arbiter)
    : QFrame(), QuickView(arbiter, "Climate_Combo", this, true)
{
}

void ClimateControls::init()
{
    /*LAYOUT */
    HVAC = new MalibuHVAC();
    int ico_size = 32;
    horizontalLayout = new QHBoxLayout(this);

    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout->setSpacing(0);
    horizontalLayout->addStretch();

    autoButton = new QPushButton(this);
    autoButton->setText("auto");
    horizontalLayout->addWidget(autoButton);

    tempDownButton = new QPushButton(this);
    tempDownButton->setObjectName(QString::fromUtf8("tempDownButton"));
    tempDownButton->setMaximumSize(QSize(32, 16777215));
    tempDownButton->setFlat(true);
    tempDownButton->setText("-");
    horizontalLayout->addWidget(tempDownButton);

    tempLabel = new QLabel(this);
    tempLabel->setObjectName(QString::fromUtf8("tempLabel"));
    tempLabel->setAlignment(Qt::AlignCenter);
    tempLabel->setText("20°C");
    horizontalLayout->addWidget(tempLabel);

    tempUpButton = new QPushButton(this);
    tempUpButton->setObjectName(QString::fromUtf8("tempUpButton"));
    tempUpButton->setMaximumSize(QSize(32, 16777215));
    tempUpButton->setFlat(true);
    tempUpButton->setText("+");
    horizontalLayout->addWidget(tempUpButton);

    driverSeatButton = new QPushButton(this);
    driverSeatButton->setFlat(true);
    driverSeatButton->setCheckable(true);
    driverSeatButton->setProperty("color_hint", true);
    driverSeatButton->setProperty("base_color", true);
    this->arbiter.forge().iconize(QString::fromUtf8("heated_seat"), driverSeatButton, ico_size);
    horizontalLayout->addWidget(driverSeatButton);

    horizontalLayout->addStretch();

    acButton = new QPushButton(this);
    acButton->setFlat(true);
    acButton->setCheckable(true);
    acButton->setProperty("color_hint", true);
    acButton->setProperty("base_color", true);
    this->arbiter.forge().iconize(QString::fromUtf8("ac_off"), acButton, ico_size);
    horizontalLayout->addWidget(acButton);

    climateState = new ClimateState(this->arbiter, this);
    horizontalLayout->addWidget(climateState);
    auto climate_dialog = new ClimateDialog(this->arbiter, false, climateState);

    fanSpeedDownButton = new QPushButton(this);
    fanSpeedDownButton->setText("-");
    horizontalLayout->addWidget(fanSpeedDownButton);
    fan_speed = new StepMeter(this->arbiter, this);
    fan_speed->set_steps(4);
    fan_speed->set_bars(0);
    horizontalLayout->addWidget(fan_speed);
    fanSpeedUpButton = new QPushButton(this);
    fanSpeedUpButton->setText("+");
    horizontalLayout->addWidget(fanSpeedUpButton);

    defrostButton = new QPushButton(this);
    defrostButton->setFlat(true);
    defrostButton->setCheckable(true);
    defrostButton->setProperty("color_hint", true);
    defrostButton->setProperty("base_color", true);
    this->arbiter.forge().iconize(QString::fromUtf8("rear-window-defrost"), defrostButton, ico_size);
    horizontalLayout->addWidget(defrostButton);

    recircButton = new QPushButton(this);
    recircButton->setFlat(true);
    recircButton->setCheckable(true);
    recircButton->setProperty("color_hint", true);
    recircButton->setProperty("base_color", true);
    this->arbiter.forge().iconize(QString::fromUtf8("recirc_auto"), recircButton, ico_size);
    horizontalLayout->addWidget(recircButton);

    horizontalLayout->addStretch();

    passengerSeatButton = new QPushButton(this);
    passengerSeatButton->setFlat(true);
    passengerSeatButton->setCheckable(true);
    passengerSeatButton->setProperty("color_hint", true);
    passengerSeatButton->setProperty("base_color", true);
    this->arbiter.forge().iconize(QString::fromUtf8("heated_seat"), passengerSeatButton, ico_size);
    horizontalLayout->addWidget(passengerSeatButton);

    passengerTempDownButton = new QPushButton(this);
    passengerTempDownButton->setObjectName(QString::fromUtf8("tempDownButton"));
    passengerTempDownButton->setMaximumSize(QSize(32, 16777215));
    passengerTempDownButton->setFlat(true);
    passengerTempDownButton->setText("-");
    horizontalLayout->addWidget(passengerTempDownButton);

    passengerTempLabel = new QLabel(this);
    passengerTempLabel->setObjectName(QString::fromUtf8("tempLabel"));
    passengerTempLabel->setAlignment(Qt::AlignCenter);
    passengerTempLabel->setText("20°C");
    horizontalLayout->addWidget(passengerTempLabel);

    passengerTempUpButton = new QPushButton(this);
    passengerTempUpButton->setObjectName(QString::fromUtf8("tempUpButton"));
    passengerTempUpButton->setMaximumSize(QSize(32, 16777215));
    passengerTempUpButton->setFlat(true);
    passengerTempUpButton->setText("+");
    horizontalLayout->addWidget(passengerTempUpButton);

    syncButton = new QPushButton(this);
    syncButton->setText("sync");
    horizontalLayout->addWidget(syncButton);

    horizontalLayout->addStretch();

    connect(tempUpButton, &QPushButton::clicked, [this]
    {
        HVAC->increaseDriverTemp();
    });

    connect(tempDownButton, &QPushButton::clicked, [this]
    {
        HVAC->decreaseDriverTemp();
    });

    connect(passengerTempUpButton, &QPushButton::clicked, [this]
    {
        HVAC->increasePassengerTemp();
    });

    connect(passengerTempDownButton, &QPushButton::clicked, [this]
    {
        HVAC->decreasePassengerTemp();
    });

    connect(acButton, &QPushButton::clicked, [this]
    {
        HVAC->toggleAC();
    });

    connect(fanSpeedDownButton, &QPushButton::clicked, [this]
    {
        HVAC->decreaseFan();
    });

    connect(fanSpeedUpButton, &QPushButton::clicked, [this]
    {
        HVAC->increaseFan();
    });

    connect(recircButton, &QPushButton::clicked, [this]
    {
        HVAC->toggleRecirc();
    });


    connect(defrostButton, &QAbstractButton::toggled, [this](bool enabled)
    {
               //this->defrostButton->setVisible(this->arbiter.vic().climate->toggleRDefrost());
              qDebug() << "toggled defrost" << enabled;
    });

    connect(HVAC, &MalibuHVAC::driverTempChanged,[this](int temp){
        this->tempLabel->setText(QString::number(temp));
    });

    connect(HVAC, &MalibuHVAC::passengerTempChanged, [this](int temp){
        this->passengerTempLabel->setText(QString::number(temp));
    });

    connect(HVAC, &MalibuHVAC::ventModeChanged, [this](VentMode mode){
        if(mode == VentMode::defrost){
            climateState->toggle_body(false);
            climateState->toggle_feet(false);
            climateState->toggle_defrost(true);
        }
        if(mode == VentMode::defrostFeet){
            climateState->toggle_body(false);
            climateState->toggle_feet(true);
            climateState->toggle_defrost(true);
        }
        if(mode == VentMode::feet){
            climateState->toggle_body(false);
            climateState->toggle_feet(true);
            climateState->toggle_defrost(false);
        }
        if(mode == VentMode::panel){
            climateState->toggle_body(true);
            climateState->toggle_feet(false);
            climateState->toggle_defrost(false);
        }
        if(mode == VentMode::panelFeet){
            climateState->toggle_body(true);
            climateState->toggle_feet(true);
            climateState->toggle_defrost(false);
        }
       
    });
                //this->setTemp(
                 //this->arbiter.vic().climate->decreaseTemp()); });

    //   connect(fan_speed, &StepMeter::pressed, [this]() {
    //   if(this->fan_speed->get_value() == 4)
    //       this->fan_speed->set_bars(0);
    //   else
    //       this->fan_speed->set_bars(this->fan_speed->get_value() + 1);

    //  });

} // setupUi

void ClimateControls::setTemp(double temp)
{
    qDebug("Setting temp");
    this->tempLabel->setText(QString::number(temp) + QString("°C"));
}
