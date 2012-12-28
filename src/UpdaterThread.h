#ifndef __UPDATERTHREAD_H
#define __UPDATERTHREAD_H

#include <vector>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include <IceUtil/Handle.h>
#include <sensors.h>

// 100ms, 10Hz
#define UPDATE_INTERVAL (100 * 1000)


class jsJoystick;
typedef std::vector<jsJoystick*> jslist_t;


class UpdaterThread : public IceUtil::Thread
{
 public:
  UpdaterThread(sensors::SensorFrameReceiverPrx cb,
		IceUtil::Mutex &m,
		sensors::SensorFrame &sf,
		jslist_t &js);

  virtual void run(); // Thread entry point
  void requestStop(); // Instruct thread loop to quit

  // Updates sensor_frame.
  // Rreturns true if there are differences between old an new
  // values. Otherwise false.
  sensors::SensorFrame querySensors();

 private:
  bool should_stop;
  sensors::SensorFrameReceiverPrx sensor_cb;
  IceUtil::Mutex &mutex;
  sensors::SensorFrame &sensor_frame;
  jslist_t &joysticks;
};

typedef IceUtil::Handle<UpdaterThread> UpdaterThreadPtr;


#endif // __UPDATERTHREAD_H
