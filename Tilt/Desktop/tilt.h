#ifndef TILT_H
#define TILT_H

#include <QDialog>
class QLCDNumber;

namespace Ui {
  class Tilt;
}

class QSerialPort;
class QTimer;

class Tilt : public QDialog
{
  Q_OBJECT

public:
  explicit Tilt(QWidget *parent = 0);
  ~Tilt();

private slots:
  void on_startButton_clicked();
  void Tick();
  void on_stopButton_clicked();
  void ReadSerial();

private:
  void ProcessTimer(int &timer, int &timerTotal);
  void RefreshLCDs();
  void startDeviceDiscovery();

private:
  enum ETimers
  {
    SEATED = 0,
    SEATED_TOTAL,
    STANDING,
    STANDING_TOTAL,
    TOTAL,
    TIMERS_COUNT
  };

  enum ETilt
  {
    UP,
    DOWN
  };

  Ui::Tilt *ui;
  QSerialPort *arduino;
  static const quint16 arduino_uno_vendor_id = 9025;
  static const quint16 arduino_uno_product_id = 1;
  QString arduino_port_name;
  bool arduino_is_available;
  QTimer *m_TickTimer;
  QList<int> timers;
  QList<QLCDNumber *> LCDs;
  ETilt m_CurrentTilt;
  ETilt m_PreviousTilt;
  QString m_serialBuffer;
  int m_Proximity;
  int m_AbsenceSeconds;
};

#endif // TILT_H
