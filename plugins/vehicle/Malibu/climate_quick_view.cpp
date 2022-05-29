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
#include "vent_mode.hpp"

ClimateControls::ClimateControls(Arbiter &arbiter)
    : QFrame(), QuickView(arbiter, "Climate_Combo", this, true)
{
}

void ClimateControls::init()
{
    /*LAYOUT */
    HVAC = new MalibuHVAC();
    int icon_size = 26 * arbiter.layout().scale;
    horizontalLayout = new QHBoxLayout(this);

    auto_icon_engine_on = new StylizedIconEngine(this->arbiter, QString(":/icons/auto.svg"), true);
    auto_on_icon = QIcon(auto_icon_engine_on);

    ac_max_icon_engine = new StylizedIconEngine(this->arbiter, QString(":/icons/ac_max.svg"), true);
    ac_max_icon = QIcon(ac_max_icon_engine);
    
    ac_eco_icon_engine = new StylizedIconEngine(this->arbiter, QString(":/icons/ac_eco.svg"), true);
    ac_eco_icon = QIcon(ac_eco_icon_engine);

    ac_off_icon_engine = new StylizedIconEngine(this->arbiter, QString(":/icons/ac_off.svg"), false);
    ac_off_icon = QIcon(ac_off_icon_engine);

    seat_off_icon_engine = new StylizedIconEngine(this->arbiter, QString(":/icons/heated_seat_off.svg"), false);
    seat_off_icon = QIcon(seat_off_icon_engine);

    seat_1_icon_engine =  new StylizedIconEngine(this->arbiter, QString(":/icons/heated_seat_1.svg"), true);
    seat_1_icon = QIcon(seat_1_icon_engine);

    seat_2_icon_engine =  new StylizedIconEngine(this->arbiter, QString(":/icons/heated_seat_2.svg"), true);
    seat_2_icon = QIcon(seat_2_icon_engine);

    seat_3_icon_engine =  new StylizedIconEngine(this->arbiter, QString(":/icons/heated_seat_3.svg"), true);
    seat_3_icon = QIcon(seat_3_icon_engine);

    defrost_icon_engine = new StylizedIconEngine(this->arbiter, QString(":/icons/rear-window-defrost.svg"), true);
    defrost_icon = QIcon(defrost_icon_engine);

    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout->setSpacing(0);
    horizontalLayout->addStretch();

    autoButton = new QPushButton(this);
    autoButton->setCheckable(true);
    autoButton->setFlat(true);
    this->arbiter.forge().iconize(auto_on_icon, autoButton, icon_size);
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
    this->arbiter.forge().iconize(QString::fromUtf8("heated_seat"), driverSeatButton, icon_size);
    horizontalLayout->addWidget(driverSeatButton);

    horizontalLayout->addStretch();

    acButton = new QPushButton(this);
    acButton->setFlat(true);
    acButton->setCheckable(true);
    this->arbiter.forge().iconize(QString::fromUtf8("ac_off"), acButton, icon_size);
    horizontalLayout->addWidget(acButton);

    climateState = new ClimateState(this->arbiter, this);
    horizontalLayout->addWidget(climateState);

    fanSpeedDownButton = new QPushButton(this);
    fanSpeedDownButton->setFlat(true);
    fanSpeedDownButton->setText("-");
    horizontalLayout->addWidget(fanSpeedDownButton);
    fan_speed = new StepMeter(this->arbiter, this);
    fan_speed->set_steps(4);
    fan_speed->set_bars(0);
    horizontalLayout->addWidget(fan_speed);
    fanSpeedUpButton = new QPushButton(this);
    fanSpeedUpButton->setFlat(true);
    fanSpeedUpButton->setText("+");
    horizontalLayout->addWidget(fanSpeedUpButton);

    defrostButton = new QPushButton(this);
    defrostButton->setFlat(true);
    defrostButton->setCheckable(true);
    this->arbiter.forge().iconize(defrost_icon, defrostButton, icon_size);
    horizontalLayout->addWidget(defrostButton);

    recircButton = new QPushButton(this);
    recircButton->setFlat(true);
    this->arbiter.forge().iconize(QString::fromUtf8("recirc_auto"), recircButton, icon_size);
    horizontalLayout->addWidget(recircButton);

    horizontalLayout->addStretch();

    passengerSeatButton = new QPushButton(this);
    passengerSeatButton->setFlat(true);
    passengerSeatButton->setCheckable(true);
    this->arbiter.forge().iconize(QString::fromUtf8("heated_seat"), passengerSeatButton, icon_size);
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
    syncButton->setCheckable(true);
    syncButton->setFlat(true);
    this->arbiter.forge().iconize(auto_on_icon, syncButton, icon_size);
    horizontalLayout->addWidget(syncButton);

    horizontalLayout->addStretch();

    /***** BUTTON EVENTS ******/
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

    connect(climateState, &ClimateState::clicked, [this]()
    {
        HVAC->toggleVentMode();
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

    connect(defrostButton, &QPushButton::clicked, [this]()
    {
        HVAC->toggleRearDefrost();
    });

    connect(driverSeatButton, &QPushButton::clicked, [this]()
    {
        HVAC->toggleDriverHeatedSeat();
    });

    connect(passengerSeatButton, &QPushButton::clicked, [this]()
    {
        HVAC->togglePassengerHeatedSeat();
    });

    connect(autoButton, &QPushButton::clicked, [this]()
    {
        HVAC->toggleDriverAuto();
    });

    connect(syncButton, &QPushButton::clicked, [this]()
    {
        HVAC->togglePassengerSync();
    });

    /******* HVAC EVENTS ********/
    connect(HVAC, &MalibuHVAC::driverTempChanged,[this](uint temp){
        this->tempLabel->setText(QString::number(temp) + "°C");
        if(temp == 253) this->tempLabel->setText("MIN");
        if(temp == 254) this->tempLabel->setText("MAX");
    });

    connect(HVAC, &MalibuHVAC::passengerTempChanged, [this](uint temp){
        this->passengerTempLabel->setText(QString::number(temp) + "°C");
        if(temp == 253) this->passengerTempLabel->setText("MIN");
        if(temp == 254) this->passengerTempLabel->setText("MAX");
    });

    connect(HVAC, &MalibuHVAC::ventModeChanged, [this](VentMode mode){
        if(mode == VentMode::auto_vent){
            climateState->toggle_body(false);
            climateState->toggle_feet(false);
            climateState->toggle_defrost(false);
            climateState->toggle_auto(true);
        }
        if(mode == VentMode::defrost){
            climateState->toggle_auto(false);
            climateState->toggle_body(false);
            climateState->toggle_feet(false);
            climateState->toggle_defrost(true);
        }
        if(mode == VentMode::defrostFeet){
            climateState->toggle_auto(false);
            climateState->toggle_body(false);
            climateState->toggle_feet(true);
            climateState->toggle_defrost(true);
        }
        if(mode == VentMode::feet){
            climateState->toggle_auto(false);
            climateState->toggle_body(false);
            climateState->toggle_feet(true);
            climateState->toggle_defrost(false);
        }
        if(mode == VentMode::panel){
            climateState->toggle_auto(false);
            climateState->toggle_body(true);
            climateState->toggle_feet(false);
            climateState->toggle_defrost(false);
        }
        if(mode == VentMode::panelFeet){
            climateState->toggle_auto(false);
            climateState->toggle_body(true);
            climateState->toggle_feet(true);
            climateState->toggle_defrost(false);
        }
       
    });

    connect(HVAC, &MalibuHVAC::acChanged,[this, icon_size](ACLevel level)
    {
        switch(level)
        {
            case max:
                {
                    this->arbiter.forge().iconize(ac_max_icon, acButton, icon_size);
                    acButton->setChecked(true);
                }
                break;
                
            case eco:
                {
               
                    this->arbiter.forge().iconize(ac_eco_icon, acButton, icon_size);
                    acButton->setChecked(true);
                }
                break;

            default:
                this->arbiter.forge().iconize(ac_off_icon, acButton, icon_size);
                acButton->setChecked(false);
                break;

        }
    });

    connect(HVAC, &MalibuHVAC::driverAutoChanged, [this](bool enabled){
        if(enabled){
            autoButton->setChecked(true);
        } else {
            autoButton->setChecked(false);
        }
    });

    connect(HVAC, &MalibuHVAC::passengerSyncChanged, [this](bool enabled){
        if(enabled){
            syncButton->setChecked(true);
        } else {
            syncButton->setChecked(false);
        }
    });
    
    connect(HVAC, &MalibuHVAC::driverHeatedSeatChanged, [this, icon_size](int level){
        if(level == 0){      
            this->arbiter.forge().iconize(seat_off_icon, driverSeatButton, icon_size);
            driverSeatButton->setChecked(false);
        }

        if(level == 1){
            this->arbiter.forge().iconize(seat_1_icon, driverSeatButton, icon_size);
            driverSeatButton->setChecked(true);
        }

        if(level == 2){
            this->arbiter.forge().iconize(seat_2_icon, driverSeatButton, icon_size);
            driverSeatButton->setChecked(true);
        }

        if(level == 3){
            this->arbiter.forge().iconize(seat_3_icon, driverSeatButton, icon_size);
            driverSeatButton->setChecked(true);
        }
    });

    connect(HVAC, &MalibuHVAC::passengerHeatedSeatChanged, [this, icon_size](int level){
        if(level == 0){  
            this->arbiter.forge().iconize(seat_off_icon, passengerSeatButton, icon_size);
            passengerSeatButton->setChecked(false);
        }

        if(level == 1){
            this->arbiter.forge().iconize(seat_1_icon, passengerSeatButton, icon_size);
            passengerSeatButton->setChecked(true);
        }

        if(level == 2){
            this->arbiter.forge().iconize(seat_2_icon, passengerSeatButton, icon_size);
            passengerSeatButton->setChecked(true);
        }

        if(level == 3){
            this->arbiter.forge().iconize(seat_3_icon, passengerSeatButton, icon_size);
            passengerSeatButton->setChecked(true);
        }
    });
    
    connect(HVAC, &MalibuHVAC::recircChanged, [this, icon_size](RecircMode mode)
    {
        if(mode == RecircMode::auto_recirc){
             this->arbiter.forge().iconize(QString::fromUtf8("recirc_auto"), recircButton, icon_size);
        } else {
             this->arbiter.forge().iconize(QString::fromUtf8("recirc_internal"), recircButton, icon_size);
        }
    });

    connect(HVAC, &MalibuHVAC::rearDefrostChanged, [this](bool enabled)
    {
        if(enabled){
            defrostButton->setChecked(true);
        }else {
            defrostButton->setChecked(false);
        }
    });

} 
