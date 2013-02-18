/* Copyright (c) 2013 Andrey Nechypurenko
   See the file COPYING for copying permission.
*/
#include "SensorGroupI.h"
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <plib/js.h> // Joystick support

using namespace std;

SensorGroupI::SensorGroupI(admin::StatePrx &prx)
  : state_prx(prx)
{
  jsInit();
  // Discover how many joysticks we have and initialize them
  size_t id = 0;
  while(true)
    {
      this->joysticks.push_back(new jsJoystick(id));
      jsJoystick *j = this->joysticks.back();
      if(j->notWorking())
	{
	  delete j;
	  this->joysticks.pop_back();
	  break;
	}
      else
	{
	  const sensors::SensorDescription descr = 
	    {
	      // sensor unique (for the group) id
	      id, sensors::Joystick,
	      // data range
	      -1, 1,
	      // recommended sensor refresh rate
	      1000000 / UPDATE_INTERVAL,
	      "Joystick", j->getName()
	    };
	  this->sensors_descr.push_back(descr);
	  // Initialize sensor_frame arrays
	  this->sensor_frame.push_back(sensors::SensorData());
	  sensors::SensorData &data = this->sensor_frame.back();
	  data.sensorid = id;
	  data.intdata.push_back(0); // buttons
	  data.floatdata.resize(j->getNumAxes(), 0.0);
	}
      ++id;
    }
  if(this->joysticks.empty())
    throw runtime_error("No joysticks found");
}


SensorGroupI::~SensorGroupI()
{
  this->requestStop();

  for(jslist_t::iterator j = this->joysticks.begin();
      j != this->joysticks.end(); ++j)
    delete *j;
}


void 
SensorGroupI::requestStop()
{
  if(this->updater_thread && this->updater_thread->isAlive())
    {
      this->updater_thread->requestStop();
      this->updater_thread->getThreadControl().join();
    }
}


void 
SensorGroupI::requestStart()
{
  if(this->updater_thread && this->updater_thread->isAlive())
    return;

  this->updater_thread = new UpdaterThread(this->sensor_cb,
					   this->mutex,
					   this->sensor_frame,
					   this->joysticks);
  this->updater_thread->start();
}


admin::StatePrx
SensorGroupI::getStateInterface(const Ice::Current&)
{
  return this->state_prx;
}


sensors::SensorDescriptionSeq
SensorGroupI::getSensorDescription(const Ice::Current&)
{
  return this->sensors_descr;
}


sensors::SensorFrame
SensorGroupI::getCurrentValues(const Ice::Current&)
{
  this->updater_thread->querySensors();
  IceUtil::Mutex::Lock lock(this->mutex);
  return this->sensor_frame;
}


bool
SensorGroupI::setSensorReceiver(const sensors::SensorFrameReceiverPrx& callback,
				const Ice::Current&)
{
  // Stopping updater thread. We assume, that after changing receiver,
  // the State's start() method should becalled to resume sending
  // joystick data
  this->requestStop();
  this->sensor_cb = sensors::SensorFrameReceiverPrx::uncheckedCast(callback->ice_oneway()->ice_timeout(2000));;

  return true;
}


void
SensorGroupI::cleanSensorReceiver(const Ice::Current&)
{
  this->requestStop();
  this->sensor_cb = 0;
}
