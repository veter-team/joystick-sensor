/* Copyright (c) 2013 Andrey Nechypurenko
   See the file COPYING for copying permission.
*/
#ifndef __SENSORGROUPI_H
#define __SENSORGROUPI_H

#include <IceUtil/Handle.h>
#include <sensors.h>
#include "UpdaterThread.h"


class SensorGroupI : public sensors::SensorGroup
{
 public:
  SensorGroupI(admin::StatePrx &prx);
  ~SensorGroupI();

 public:
  void requestStart(); // Starts updater thread
  void requestStop(); // Request updater thread to quit

 public:
  virtual admin::StatePrx getStateInterface(const Ice::Current& = Ice::Current());

  virtual sensors::SensorDescriptionSeq getSensorDescription(const Ice::Current& = Ice::Current());

  virtual sensors::SensorFrame getCurrentValues(const Ice::Current& = Ice::Current());

  virtual bool setSensorReceiver(const sensors::SensorFrameReceiverPrx& callback,
				 const Ice::Current& = ::Ice::Current());

  virtual void cleanSensorReceiver(const Ice::Current& = ::Ice::Current());

 private:
  admin::StatePrx state_prx;
  sensors::SensorFrameReceiverPrx sensor_cb;
  IceUtil::Mutex mutex;
  sensors::SensorFrame sensor_frame;
  sensors::SensorDescriptionSeq sensors_descr;
  jslist_t joysticks;
  UpdaterThreadPtr updater_thread;
};

typedef IceUtil::Handle<SensorGroupI> SensorGroupIPtr;

#endif // __SENSORGROUPI_H
