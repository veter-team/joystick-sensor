/* Copyright (c) 2013 Andrey Nechypurenko
   See the file COPYING for copying permission.
*/
#include "IceSvc.h"
#include "SensorAdminI.h"

using namespace std;


IceSvc::IceSvc()
{
}


bool 
IceSvc::start(int argc, char *argv[], int &status)
{
  Ice::ObjectAdapterPtr adapter = 
    this->communicator()->createObjectAdapter("joystick-sensor");

  SensorAdminIPtr adm = new SensorAdminI();
  Ice::ObjectPrx obj = 
    adapter->add(adm, this->communicator()->stringToIdentity("joystick-sensor-admin"));
  admin::StatePrx state_prx = admin::StatePrx::uncheckedCast(obj);

  this->sensor_group = new SensorGroupI(state_prx);
  adm->setSensorGroup(this->sensor_group);
  adapter->add(this->sensor_group, 
	       this->communicator()->stringToIdentity("joystick-sensor"));

  adapter->activate();
  
  status = EXIT_SUCCESS;
  return true;
}


bool 
IceSvc::stop()
{
  this->sensor_group->requestStop();
  return Ice::Service::stop();
}
