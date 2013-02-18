/* Copyright (c) 2013 Andrey Nechypurenko
   See the file COPYING for copying permission.
*/
#include "UpdaterThread.h"
#include <plib/js.h> // Joystick support


UpdaterThread::UpdaterThread(sensors::SensorFrameReceiverPrx cb,
			     IceUtil::Mutex &m,
			     sensors::SensorFrame &sf,
			     jslist_t &js)
  : sensor_cb(cb), mutex(m), sensor_frame(sf), joysticks(js)
{
}


void 
UpdaterThread::requestStop()
{
  this->should_stop = true;
}


void 
UpdaterThread::run()
{
  this->should_stop = false;
  while(!this->should_stop)
    {
      try
	{
	  if(this->sensor_cb)
	    {
	      sensors::SensorFrame new_frame = this->querySensors();
	      if(!new_frame.empty())
		  this->sensor_cb->nextSensorFrame(new_frame);
	      usleep(UPDATE_INTERVAL);
	    }
	  else
	    this->should_stop = true;
	}
      catch(const Ice::Exception& ex)
	{
	  //cout << ex << '\n';
	  //cout << "Forgetting sensor callback receiver\n";
	  this->sensor_cb = 0;
	  this->should_stop = true;
	}
    }
}
 

sensors::SensorFrame 
UpdaterThread::querySensors()
{
  sensors::SensorFrame retval;
  IceUtil::Mutex::Lock lock(this->mutex);

  sensors::SensorFrame::iterator sf = this->sensor_frame.begin();
  for(jslist_t::iterator ji = this->joysticks.begin();
      ji != this->joysticks.end(); ++ji)
    {
      jsJoystick *j = *ji;
      sensors::SensorData new_data;
      new_data.intdata.resize(1, 0);
      new_data.floatdata.resize(sf->floatdata.size(), 0.0);
      j->read(&(new_data.intdata[0]), &(new_data.floatdata[0]));
      if(new_data.intdata != sf->intdata
	 || new_data.floatdata != sf->floatdata)
	{
	  sf->intdata = new_data.intdata;
	  sf->floatdata = new_data.floatdata;
	  new_data.sensorid = sf->sensorid;
	  retval.push_back(new_data);
	}
      ++sf;
    }
  return retval;
}
