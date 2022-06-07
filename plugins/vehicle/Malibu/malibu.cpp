#include <QDebug>
#include <QTimer>

#include "malibu.hpp"

class Arbiter;

Malibu::~Malibu()
{
    if (this->climate)
        delete this->climate;
    if (this->vehicle)
        delete this->vehicle;
}

QList<QWidget *> Malibu::widgets()
{
    QList<QWidget *> tabs;
    if (this->vehicle)
        tabs.append(this->vehicle);
    return tabs;
}

bool Malibu::init(ICANBus *)
{
    if (this->arbiter)
    {
        this->vehicle = new Vehicle(*this->arbiter);
        this->vehicle->pressure_init("psi", 35);
        this->vehicle->disable_sensors();
        this->vehicle->rotate(270);

        this->climate = new Climate(*this->arbiter);
        this->climate->max_fan_speed(4);

        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]
                {
            static bool toggle = false;

            switch(rand() % 50) {
                case 0:
                    this->climate->airflow(Airflow::OFF);
                    break;
                case 1:
                    this->climate->airflow(Airflow::DEFROST);
                    break;
                case 2:
                    this->climate->airflow(Airflow::BODY);
                    break;
                case 3:
                    this->climate->airflow(Airflow::FEET);
                    break;
                case 4:
                    this->climate->airflow(Airflow::DEFROST | Airflow::BODY);
                    break;
                case 5:
                    this->climate->airflow(Airflow::DEFROST | Airflow::FEET);
                    break;
                case 6:
                    this->climate->airflow(Airflow::BODY | Airflow::FEET);
                    break;
                case 7:
                    this->climate->airflow(Airflow::DEFROST | Airflow::BODY | Airflow::FEET);
                    break;
                case 8:
                    this->climate->fan_speed((rand() % 4) + 1);
                    break;
                case 9:
                    this->vehicle->sensor(Position::FRONT_LEFT, rand() % 5);
                    break;
                case 10:
                    this->vehicle->sensor(Position::FRONT_MIDDLE_LEFT, rand() % 5);
                    break;
                case 11:
                    this->vehicle->sensor(Position::FRONT_MIDDLE_RIGHT, rand() % 5);
                    break;
                case 12:
                    this->vehicle->sensor(Position::FRONT_RIGHT, rand() % 5);
                    break;
                case 13:
                    this->vehicle->sensor(Position::BACK_LEFT, rand() % 5);
                    break;
                case 14:
                    this->vehicle->sensor(Position::BACK_MIDDLE_LEFT, rand() % 5);
                    break;
                case 15:
                    this->vehicle->sensor(Position::BACK_MIDDLE_RIGHT, rand() % 5);
                    break;
                case 16:
                    this->vehicle->sensor(Position::BACK_RIGHT, rand() % 5);
                    break;
                case 17:
                    this->vehicle->door(Position::FRONT_LEFT, toggle);
                    break;
                case 18:
                    this->vehicle->door(Position::BACK_LEFT, toggle);
                    break;
                case 19:
                    this->vehicle->door(Position::FRONT_RIGHT, toggle);
                    break;
                case 20:
                    this->vehicle->door(Position::BACK_RIGHT, toggle);
                    break;
                case 21:
                    this->vehicle->headlights(toggle);
                    break;
                case 22:
                    this->vehicle->taillights(toggle);
                    break;
                case 23:
                    this->vehicle->pressure(Position::BACK_RIGHT, (rand() % 21) + 30);
                    break;
                case 24:
                    this->vehicle->pressure(Position::BACK_LEFT, (rand() % 21) + 30);
                    break;
                case 25:
                    this->vehicle->pressure(Position::FRONT_RIGHT, (rand() % 21) + 30);
                    break;
                case 26:
                    this->vehicle->pressure(Position::FRONT_LEFT, (rand() % 21) + 30);
                    break;
                case 27:
                    this->vehicle->wheel_steer((rand() % 10) * ((rand() % 2) ? 1 : -1));
                    break;
                case 28:
                    this->vehicle->indicators(Position::LEFT, toggle);
                    break;
                case 29:
                    this->vehicle->indicators(Position::RIGHT, toggle);
                    break;
                case 30:
                    this->vehicle->hazards(toggle);
                    break;
                default:
                    toggle = !toggle;
                    break;
            } });
        // timer->start(1000);

        auto timer2 = new QTimer(this);
        connect(timer2, &QTimer::timeout, [this]
                {
            if (rand() % 10 == 1) {
                this->climate->left_temp((rand() % 20) + 60);
                this->climate->right_temp((rand() % 20) + 60);
            } });
        // timer2->start(1000);
        this->climate_controls = new ClimateControls(*arbiter);
        this->arbiter->add_quick_view(climate_controls, true);
        //   this->arbiter->add_quick_view(this->climate_combo);
        // this->arbiter->layout().control_bar.add_quick_view(this->climate_combo);

        return true;
    }

    return false;
}

MalibuHVAC::MalibuHVAC()
{

    if (!serial_.setBaudRate(QSerialPort::Baud115200))
        qDebug() << "[HVAC][LIN] Connection error: " << serial_.errorString();
    if (!serial_.setDataBits(QSerialPort::Data8))
        qDebug() << "[HVAC][LIN] Connection error: " << serial_.errorString();
    if (!serial_.setParity(QSerialPort::NoParity))
        qDebug() << "[HVAC][LIN] Connection error: " << serial_.errorString();
    if (!serial_.setFlowControl(QSerialPort::HardwareControl))
        qDebug() << "[HVAC][LIN] Connection error: " << serial_.errorString();
    if (!serial_.setStopBits(QSerialPort::OneStop))
        qDebug() << "[HVAC][LIN] Connection error: " << serial_.errorString();

    serial_.setPortName("ttyS0");
    if(serial_.open(QIODevice::ReadWrite)){
        qDebug() << "[HVAC][LIN] Connection successfully established on port ttyS0";
   
    } else {
        qDebug() << "[HVAC][LIN] Connection error: " << serial_.errorString();
  
    }

    connect(&serial_, &QSerialPort::readyRead, [&]
    {
        qDebug() << "[HVAC][LIN] data available: " << serial_.bytesAvailable();
        QByteArray datas = serial_.readAll();
        qDebug() << datas;
        read_buffer.append(datas);
        process_serial_data();
    });

    connect(&serial_,static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
        [&](QSerialPort::SerialPortError error)
    {
        qDebug() << "[HVAC][LIN] ERROR: " << error;
    });
}

void MalibuHVAC::send_lin_data(QByteArray data)
{
    QByteArray lin_frame = QByteArray(10,0);
    lin_frame.replace(1, 8, data);
    lin_frame[0] = 0x55;
    lin_frame[9] = 0x0A;
    serial_.write(lin_frame);
}

void MalibuHVAC::increaseDriverTemp(){
    QByteArray ba = QByteArray(8,0);
    ba[5] = 0x01;
    send_lin_data(ba);
}

void MalibuHVAC::decreaseDriverTemp(){
    QByteArray ba = QByteArray(8,0);
    ba[5] = 0x7F;
    send_lin_data(ba);
}

void MalibuHVAC::increasePassengerTemp()
{
    QByteArray ba = QByteArray(8,0);
    ba[6] = 0x01;
    send_lin_data(ba);
}

void MalibuHVAC::decreasePassengerTemp()
{
    QByteArray ba = QByteArray(8,0);
    ba[6] = 0x7F;
    send_lin_data(ba);
}

void MalibuHVAC::increaseFan()
{
    QByteArray ba = QByteArray(8,0);
    ba[7] = 0x01;
    send_lin_data(ba);
}

void MalibuHVAC::decreaseFan()
{
    QByteArray ba = QByteArray(8,0);
    ba[7] = 0x7F;
    send_lin_data(ba);
}

void MalibuHVAC::toggleVentMode()
{

    switch(current_status.ventMode()){
        case VentMode::defrost:
            setVentMode(VentMode::defrostFeet);
            break;
        case VentMode::defrostFeet:
            setVentMode(VentMode::feet);
            break;
        case VentMode::feet:
            setVentMode(VentMode::panelFeet);
            break;
        case VentMode::auto_vent:
        default:
        case VentMode::panelFeet:
            setVentMode(VentMode::panel);
            break;
        case VentMode::panel:
            setVentMode(VentMode::defrost);
            break;

    }
}

void MalibuHVAC::setVentMode(VentMode mode)
{
    if(mode == VentMode::defrostFeet){
        QByteArray ba = QByteArray(8,0);
        ba[4] = 0x02;//this right?
        send_lin_data(ba);
    } else {
        QByteArray ba = QByteArray(8,0);
        ba[1] = mode;
        send_lin_data(ba);
    }
}

void MalibuHVAC::toggleAC()
{
    QByteArray ba = QByteArray(8,0);
    ba[1] = buttonMask1::AC_FULL;
    send_lin_data(ba);
}

void MalibuHVAC::toggleRecirc()
{
    QByteArray ba = QByteArray(8,0);
    ba[1] = buttonMask1::RECIRC;
    send_lin_data(ba);
}

void MalibuHVAC::toggleDriverAuto()
{
    QByteArray ba = QByteArray(8,0);
    ba[2] = buttonMask2::DRIVER_AUTO;
    send_lin_data(ba);
}

void MalibuHVAC::togglePassengerSync()
{
    QByteArray ba = QByteArray(8,0);
    ba[2] = buttonMask2::PASSENGER_SYNC;
    send_lin_data(ba);
}

void MalibuHVAC::toggleRearDefrost()
{
    QByteArray ba = QByteArray(8,0);
    ba[1] = buttonMask1::REAR_DEFROST;
    send_lin_data(ba);
}

void MalibuHVAC::toggleDriverHeatedSeat()
{
    QByteArray ba = QByteArray(8,0);
    ba[2] = buttonMask2::DRIVER_HEATED_SEAT;
    send_lin_data(ba);
}

void MalibuHVAC::togglePassengerHeatedSeat()
{
    QByteArray ba = QByteArray(8,0);
    ba[2] = buttonMask2::PASSENGER_HEATED_SEAT;
    send_lin_data(ba);
}

void MalibuHVAC::process_serial_data()
{

    // escape if not full frame
    while (read_buffer.contains('\n'))
    {

        QByteArray bytes;
        int end_index = read_buffer.indexOf('\n');
        bool changed = false;
        for (int i = 0; i < end_index; i++)
        {
            bytes[i] = read_buffer.at(i);
            if (current_status.bytes[i] != bytes[i])
            {
                changed = true;
            }

        }
        auto frame = StatusFrame(&bytes);
        read_buffer.remove(0, end_index + 1);
        if (changed)
        {   
            if(current_status.driveTemp() != frame.driveTemp()) 
                emit driverTempChanged(frame.driveTemp());
            if(current_status.passengerTemp() != frame.passengerTemp())
                emit passengerTempChanged(frame.passengerTemp());
            if(current_status.acLevel() != frame.acLevel())
                emit acChanged(frame.acLevel());
            if(current_status.ventMode() != frame.ventMode())
                emit ventModeChanged(frame.ventMode());
            if(current_status.recircMode() != frame.recircMode())
                emit recircChanged(frame.recircMode());
            if(current_status.driverAuto() != frame.driverAuto())
                emit driverAutoChanged(frame.driverAuto());
            if(current_status.passengerSync() != frame.passengerSync())
                emit passengerSyncChanged(frame.passengerSync());
            if(current_status.rearDefrost() != frame.rearDefrost())
                emit rearDefrostChanged(frame.rearDefrost());
            if(current_status.driverHeatedSeat() != frame.driverHeatedSeat())
                emit driverHeatedSeatChanged(frame.driverHeatedSeat());
            if(current_status.passengerHeatedSeat() != frame.passengerHeatedSeat())
                emit passengerHeatedSeatChanged(frame.passengerHeatedSeat());
           
            emit statusChanged();

            current_status = frame;
        }
    }
}

StatusFrame::StatusFrame(QByteArray *ba)
{

    for (int i = 0; i < ba->count(); i++)
    {
        bytes[i] = ba->at(i);

    }
}

StatusFrame::StatusFrame()
{
    bytes = QByteArray(8,(char)0);
}

uint8_t StatusFrame::driveTemp()
{
    return bytes[0];
}

uint8_t StatusFrame::passengerTemp()
{
    return bytes[1];
}

ACLevel StatusFrame::acLevel()
{
    if(bytes[2] & statusMask2::AC_FULL) return ACLevel::max;
    if(bytes[4] & statusMask4::AC_ECO) return ACLevel::eco;
    return ACLevel::off;
}

VentMode StatusFrame::ventMode()
{
    if(bytes[6] & statusMask6::DEFROST_FEET) return VentMode::defrostFeet;
    if(bytes[2] & statusMask2::PANEL_FEET) return VentMode::panelFeet;
    if(bytes[2] & statusMask2::FEET) return VentMode::feet;
    if(bytes[2] & statusMask2::DEFROST) return VentMode::defrost;
    if(bytes[2] & statusMask2::PANEL) return VentMode::panel;
    return VentMode::auto_vent;
}

RecircMode StatusFrame::recircMode()
{
    if(bytes[2] & statusMask2::RECIRC) return RecircMode::external;
    return RecircMode::auto_recirc;
}

bool StatusFrame::driverAuto()
{
    return (bytes[4] & statusMask4::DRIVER_AUTO);
}

bool StatusFrame::passengerSync()
{
    return (bytes[4] & statusMask4::PASSENGER_SYNC);
}

bool StatusFrame::rearDefrost()
{
    return (bytes[2] & statusMask2::REAR_DEFROST);
}

int StatusFrame::driverHeatedSeat()
{

    if(bytes[3] & statusMask3::DRIVER_SEAT_HIGH &&
        bytes[3] & statusMask3::DRIVER_SEAT_MEDIUM &&
        bytes[3] & statusMask3::DRIVER_SEAT_LOW) return 3;

    if(bytes[3] & statusMask3::DRIVER_SEAT_MEDIUM &&
        bytes[3] & statusMask3::DRIVER_SEAT_LOW) return 2;

    if(bytes[3] & statusMask3::DRIVER_SEAT_LOW) return 1;

    return 0;

}

int StatusFrame::passengerHeatedSeat()
{
    if(bytes[4] & statusMask4::PASSENGER_SEAT_HIGH &&
        bytes[3] & statusMask3::PASSENGER_SEAT_MEDIUM &&
        bytes[3] & statusMask3::PASSENGER_SEAT_LOW) return 3;

    if(bytes[3] & statusMask3::PASSENGER_SEAT_MEDIUM &&
        bytes[3] & statusMask3::PASSENGER_SEAT_LOW) return 2;

    if(bytes[3] & statusMask3::PASSENGER_SEAT_LOW) return 1;

    return 0;
}
