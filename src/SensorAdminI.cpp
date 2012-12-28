#include "SensorAdminI.h"


SensorAdminI::SensorAdminI()
{
}

void 
SensorAdminI::setSensorGroup(SensorGroupIPtr &sg)
{
  this->sensor_group = sg;
}


void
SensorAdminI::start(const Ice::Current&)
{
  this->sensor_group->requestStart();
}


void
SensorAdminI::stop(const Ice::Current&)
{
  this->sensor_group->requestStop();
}
