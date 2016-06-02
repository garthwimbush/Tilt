#include "tilt.h"
#include "ui_tilt.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>

Tilt::Tilt(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Tilt),
  m_Proximity(0),
  m_AbsenceSeconds(0)
{
  m_CurrentTilt = m_PreviousTilt = DOWN;
  ui->setupUi(this);
  ;

  arduino_is_available = false;
  arduino_port_name = "";
  arduino = new QSerialPort(this);

  qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
  foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
  {
    qDebug() << "For port: " << serialPortInfo.portName();
    qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
    if(serialPortInfo.hasVendorIdentifier())
    {
      qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
    }
    qDebug() << "Has Product ID: " << serialPortInfo.hasProductIdentifier();
    if(serialPortInfo.hasProductIdentifier())
    {
      qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
    }
  }
  foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
  {
    if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
    {
      if(serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)
      {
        if(serialPortInfo.productIdentifier() == arduino_uno_product_id)
        {
          arduino_port_name = serialPortInfo.portName();
          arduino_is_available = true;
        }
      }
    }
  }

  bool status = true;
  if(arduino_is_available)
  {
    // open and configure the serialport
    arduino->setPortName(arduino_port_name);
    arduino->open(QSerialPort::ReadOnly);
    arduino->setBaudRate(QSerialPort::Baud9600);
    arduino->setDataBits(QSerialPort::Data8);
    arduino->setParity(QSerialPort::NoParity);
    arduino->setStopBits(QSerialPort::OneStop);
    arduino->setFlowControl(QSerialPort::NoFlowControl);
    status = status && QObject::connect(arduino, SIGNAL(readyRead()),
                                        this, SLOT(ReadSerial()));
    Q_ASSERT(status);
  }
  else
  {
    // give error message if not available
    QMessageBox::warning(this, "Port error", "Couldn't find the Arduino!");
  }
  m_TickTimer = new QTimer();
  status = status && connect(m_TickTimer, SIGNAL(timeout()),
                             this, SLOT(Tick()));
  Q_ASSERT(status);

  for (int i=0; i < TIMERS_COUNT; ++i)
  {
    timers.push_back(0);
  }

  LCDs.push_back(ui->sittingTimeLCD);
  LCDs.push_back(ui->sittingTimeTotalLCD);
  LCDs.push_back(ui->standingTimeLCD);
  LCDs.push_back(ui->standingTimeTotalLCD);
  LCDs.push_back(ui->totalLCD);

  RefreshLCDs();
}

Tilt::~Tilt()
{
  if(arduino->isOpen()){
      arduino->close();
  }
  delete ui;
}

void Tilt::on_startButton_clicked()
{
  m_TickTimer->start(1000);
}

void Tilt::Tick()
{
  if (m_Proximity > 120)
  {
    // Remember current timers, and start the absence timer.
    ++m_AbsenceSeconds;
  }
  else
  {
    // If the absence timer is greater than 1 minute, then pop up a dialog the
    // moment the proximity comes into range again.
    if (m_AbsenceSeconds > 60)
    {
      QString absenceString = QString("You have been gone for %1 seconds. Was this a work break?").arg(m_AbsenceSeconds);
      QMessageBox::question(this, "Work Break", absenceString);
      m_AbsenceSeconds = 0;
    }
  }
  if (m_CurrentTilt == DOWN)
  {
    ProcessTimer(timers[SEATED], timers[SEATED_TOTAL]);
  }
  else
  {
    ProcessTimer(timers[STANDING], timers[STANDING_TOTAL]);
  }
  ++timers[TOTAL];
  RefreshLCDs();
}

void Tilt::ProcessTimer(int &timer, int &timerTotal)
{
  if (m_CurrentTilt != m_PreviousTilt)
  {
    timer = 1; // Take account of 1 second switch time.
    ++timerTotal;
  }
  else
  {
    ++timer;
    ++timerTotal;
  }
  m_PreviousTilt = m_CurrentTilt;
}

void Tilt::RefreshLCDs()
{
  for (int i=0; i < TIMERS_COUNT; ++i)
  {
    QString displayTemplate;
    if (timers[i] < 60)
    {
      displayTemplate = ":ss";
    }
    else if (timers[i] < (60 * 10))
    {
      displayTemplate = "m:ss";
    }
    else if (timers[i] < (60 * 60))
    {
      displayTemplate = "mm:ss";
    }
    else if (timers[i] < (60 * 60 * 10))
    {
      displayTemplate = "h:mm:ss";
    }
    else
    {
      displayTemplate = "hh:mm:ss";
    }
    LCDs[i]->display(
          QDateTime::fromTime_t(
            timers[i]).toUTC().toString(QString(displayTemplate)));
  }
}

void Tilt::on_stopButton_clicked()
{
  m_TickTimer->stop();
}

void Tilt::ReadSerial()
{
  QByteArray serialData;
  while (arduino->canReadLine()){
    QByteArray data = arduino->readLine().trimmed();
    if (data.contains(":"))
    {
      QList<QByteArray> dataPair = data.split(':');
      m_Proximity = dataPair[0].toFloat();
      m_CurrentTilt = (dataPair[1].toInt() == 1) ? UP : DOWN;
    }
    else
    {
      m_Proximity = data.toFloat();
    }
    qDebug() << data;
  }
}
