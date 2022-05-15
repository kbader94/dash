#pragma once

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "plugins/vehicle_plugin.hpp"
#include "app/widgets/climate.hpp"
#include "app/widgets/vehicle.hpp"
#include "canbus/socketcanbus.hpp"
#include "app/arbiter.hpp"
#include "climate_quick_view.hpp"

class ClimateControls;

enum ACLevel {off, eco, max};
enum RecircMode {automatic, recirculate, external};
enum VentMode {panel, panelFeet, feet, defrost, defrostFeet};

namespace buttonModeMask1{
    enum buttonModeMask1 {
        AC_FULL = 0x02,
        PANEL = 0x04,
        FEET = 0x08,
        PANEL_FEET = 0x10,
        RECIRC = 0x20,
        DEFROST = 0x80
    };// byte[1] in frame 34
}
namespace buttonModeMask2{
enum buttonModeMask2 {
    DEFROST_FEET = 0x02
};//byte[4] in frame 34
}
namespace statusModeMask1{
enum statusModeMask1{
    AC_FULL = 0x02,
    PANEL = 0x04,
    FEET = 0x08,
    PANEL_FEET = 0x10,
    RECIRC = 0x20,
    DEFROST = 0x80
};//byte[2] in frame 36
}

namespace statusModeMask2 {
enum statusModeMask2{
    DRIVER_AUTO = 0x10,
    PASSENGER_SYNC = 0x20,
    AC_ECO = 0x40

};//byte[4] in frame 36
}

namespace {
enum statusModeMask3{
    DEFROST_FEET = 0x10
};// byte[6] in frame 36
}

class StatusFrame {

    public: 
     StatusFrame(QByteArray *byteArray);
     StatusFrame();
     int driveTemp();
     int passengerTemp();
     ACLevel acLevel();
     VentMode ventMode();
     RecircMode recircMode();
     bool driverAuto();
     bool passengerSync();
     QByteArray bytes; 

};

class Malibu : public QObject, VehiclePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehiclePlugin_iid FILE "malibu.json")
    Q_INTERFACES(VehiclePlugin)

   public:
    Malibu() {};
    ~Malibu();
    QList<QWidget *> widgets() override;
    bool init(ICANBus*) override;

   private:
    Climate *climate;
    Vehicle *vehicle;
    ClimateControls *climate_controls;
    

};

class MalibuHVAC : public QObject {
    Q_OBJECT
    
    public:
     MalibuHVAC();
     void increaseDriverTemp();
     void decreaseDriverTemp();
     void increasePassengerTemp();
     void decreasePassengerTemp();
     void increaseFan();
     void decreaseFan();
     void toggleVentMode();
     void setVentMode(VentMode mode);
     void toggleAC();
     void toggleRecirc();

    signals:

     void driverTempChanged(int temp);
     void passengerTempChanged(int temp);
     void acChanged(ACLevel level);
     void recircChanged(RecircMode mode);// get from CAN
     void driverAutoChanged(bool status);
     void passengerSyncChanged(bool status);
     void rearDefrostChanged(bool status);
     void passengerHeatedSeatChanged(int level);
     void driverHeatedSeatChanged(int level);
     void ventModeChanged(VentMode mode);
     //void fanChanged(int level); get from CAN
     void statusChanged();

    private:
     QSerialPort serial_;//LIN data
     void process_serial_data();
     QByteArray read_buffer;
     StatusFrame current_status;
     bool connected_ = false;
};
