/**
 * Behaviours - UML-like graphic programming language
 * Copyright (C) 2013 Coralbits SL & AISoy Robotics.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <sstream>

#include <ab/log.h>

#include <ab/manager.h>

#include "alarm.h"


using namespace AB;
using namespace std;
Alarm::Alarm(const char *type) : Event(type)
{
  day = 1; // 1-31
  month = 0; // 0-11
  year = 112; // 2012
  hour = 0; //0-23
  minute = 0; // 0-59
  repeatPolicy = Never;
  nodeon=0;
  noderepeat=0;
}

void Alarm::setAttr(const std::string &k, const AB::Object s)
{
  if (k == "hour") {
    hour = object2int(s);
    DEBUG("alarm hour requested: %d", hour);
    return;  
  } else if (k == "minute") {
    minute = object2int(s);
    DEBUG("alarm minute requested: %d", minute);
    return;
  } else if (k == "day") {
    day = object2int(s);
    DEBUG("alarm day requested: %d", day);
    return;
  } else if (k == "month") {
    month = object2int(s);
    DEBUG("alarm month requested: %d", month);
    return;
  } else if (k == "year") {
    year = object2int(s);
    DEBUG("alarm year requested: %d", year);
    return;
  } else if (k == "repeat") {
    repeatPolicy = (Repeat)object2int(s);
    DEBUG("alarm repeat policy requested: %d", (int)repeatPolicy);
    return;
  }
  else if(k== "nodeon"){

    //manager->removeNode(this->name());
    /*printf("%s\n", "El nombre es: ");
    printf("%s\n",this->name().c_str() );
  */
    //listevent.erase(this);
    nodeon = object2int(s);
    if(nodeon==0){
      WARNING("NODEON00000000000000000000000000000000000000000000000Xx");
      if(manageralarm){
        WARNING("EntraXx");        
       if(!manageralarm->findNode(this->name())){
         WARNING("MeteXx");
          this->setManager(manageralarm);
       }
      }
    }
    else{
      if(manageralarm){
        
        manageralarm->removeNode(this->name());
          Event *ev=manageralarm->getEvent("__alarm_manager__");
          if (ev) {
            WARNING("HOLAXXXXXXXXXXXXXXXXXXXXXx");
            manageralarm->removeNode(ev->name());
          }
      }
    }
    
    DEBUG("alarm year requested: %d", nodeon);
    return;
  }
   else if(k== "noderepeat"){
    noderepeat = object2int(s);
    DEBUG("alarm year requested: %d", noderepeat);
    return;
  }
  Event::setAttr(k,s);
}

AB::Object Alarm::attr(const std::string &key)
{
  if (key=="hour") {
    return to_object(hour);
  }
  if (key == "minute") {
    return to_object(minute);
  }
  if (key == "day") {
    return to_object(day);
  }
  if (key == "month") {
    return to_object(month);
  }
  if (key == "year") {
    return to_object(year);
  }
  if (key == "repeat") {
    return to_object((int)repeatPolicy);
  }
  if (key == "nodeon") {
    return to_object(nodeon);
  }
  if(key =="noderepeat"){
    return to_object(noderepeat);
  }
  return Event::attr(key);
}

AttrList Alarm::attrList()
{
  AttrList l=Event::attrList();
  l.push_back("hour");
  l.push_back("minute");
  l.push_back("day");
  l.push_back("month");
  l.push_back("year");
  l.push_back("repeat");
  l.push_back("nodeon");
  l.push_back("noderepeat");
  return l;
}

void Alarm::setManager(Manager *man)
{
  manageralarm=man;
  
  Event *ev=man->getEvent("__alarm_manager__");
  if (!ev) {
    WARNING("SETMANAGERXXXXXXXXXXXXXXXXXXXXXx");
    ev=new AlarmManager;
      man->addNode(ev);
    ev->setManager(man);
  }
}

AlarmManager::AlarmManager() : Event("touch"), manager(NULL)
{
  setFlags(AB::Event::Polling);
  setName("__alarm_manager__");
  triggered=false;
}


void AlarmManager::setManager(Manager* _manager)
{
  manager=_manager;
}

AB::Object AlarmManager::attr(const std::string &key)
{
  DEBUG("Last time checked was %d", (int)lastAlarm);
  if (key=="lastAlarm") {
    return to_object((int)lastAlarm);
  }
  return Node::attr(key);
}

AttrList AlarmManager::attrList()
{
  AttrList l=Node::attrList();
  l.push_back("lastAlarm");
  return l;
}


bool AlarmManager::check()
{
  if (!manager)
    return false;

  time_t rawtime;
  time ( &rawtime );

    return checkAlarm(rawtime);

}

bool AlarmManager::checkAlarm(time_t rawtime)
{
  
  struct tm * timeinfo = localtime ( &rawtime );

  if (!timeinfo || (timeinfo && rawtime == lastAlarm && triggered))
    return false;
  
  if (timeinfo && rawtime > lastAlarm && triggered)
    triggered = false;
      
  for(Event *ev: manager->getActiveEvents()) {

    Alarm *tev=dynamic_cast<Alarm*>(ev);
   // WARNING(tev->getMonth());
    if (tev && tev->getHour() == timeinfo->tm_hour && tev->getMinute() == timeinfo->tm_min ) {
      printf("%d\n",tev->getNodeon());

        switch((int)tev->getRepeatPolicy()) {
        
  	case Alarm::Never:
  	  
  	  if (tev->getDay() == timeinfo->tm_mday && 
  	      tev->getMonth() == timeinfo->tm_mon && 
  	      tev->getYear() == timeinfo->tm_year ) {      
  	    manager->notify(tev);	    
  	    triggered = true;
  	  }
  	  break;
  	  
  	case Alarm::Always:
  	    manager->notify(tev);
  	    triggered = true;
  	  break;
  	  
  	case Alarm::Weekdays:
  	  if (timeinfo->tm_wday > 0 && timeinfo->tm_wday < 6) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	  
  	case Alarm::Weekends:
  	  if (timeinfo->tm_wday == 0 || timeinfo->tm_wday == 6) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	case Alarm::Sundays:
  	  if (timeinfo->tm_wday == 0) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	case Alarm::Mondays:
  	  if (timeinfo->tm_wday == 1) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	case Alarm::Tuesdays:
  	  if (timeinfo->tm_wday == 2) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	case Alarm::Wednesdays:
  	  if (timeinfo->tm_wday == 3) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	case Alarm::Thursdays:
  	  if (timeinfo->tm_wday == 4) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	case Alarm::Fridays:
  	  if (timeinfo->tm_wday == 5) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break;
  	case Alarm::Saturdays:
  	  if (timeinfo->tm_wday == 6) {
  	    manager->notify(tev);
  	    triggered = true;
  	  }
  	  break; 
        }
      
    }      
  } 
  
  if (triggered)
    lastAlarm = rawtime;

  return triggered;
}