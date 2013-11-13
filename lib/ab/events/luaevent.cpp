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

#include <algorithm>

#include "../manager.h"
#include "luaevent.h"

using namespace AB;
using namespace std;


LUAEvent::LUAEvent(const char* type) : AB::Event(type) 
{
  cont=0;
}

Object LUAEvent::attr(const std::string& paramName)
{
  if (paramName=="code" || paramName=="check")
    return to_object(checkCode);
  if (paramName=="sync")
    return to_object(syncCode);
  if (paramName=="flags")
    return to_object(flags());
  if (paramName == "nodeon") {
        return to_object(nodeon);
      }
      if(paramName =="noderepeat"){
        return to_object(noderepeat);
      }
  return Node::attr(paramName);
}

AttrList LUAEvent::attrList()
{
  AttrList l=Node::attrList();
  l.push_back("code");
  l.push_back("sync");
  l.push_back("flags");
  l.push_back("nodeon");
  l.push_back("noderepeat");
  return l;
}

void LUAEvent::setManager(Manager* m)
{
  manager=m;
}

void LUAEvent::setAttr(const std::string& paramName, AB::Object value)
{
  //DEBUG("Setting %s:%s, flags %X", name().c_str(), paramName.c_str(), flags());
  if (paramName=="code" || paramName=="check") {
    checkCode=object2string(value);
    if (checkCode.empty())
      setFlags(flags()&~AB::Event::Polling);
    else
      setFlags(flags()|AB::Event::Polling);
  } else if (paramName=="sync") {
    syncCode=object2string(value);
    if (syncCode.empty())
      setFlags(flags()&~AB::Event::NeedSync);
    else
      setFlags(flags()|AB::Event::NeedSync);
    return;
  } else if (paramName=="flags") {
    setFlags(object2int(value));
    return;
  } 
  else if(paramName== "nodeon"){
        nodeon = object2int(value);  
        DEBUG("%d",nodeon );
        if(nodeon==0){
          
          if(manager){
            WARNING("Va a introducir el evento");        
            if(!manager->findNode(this->name())){
              WARNING("Mete el evento");
              manager->addEvent(event);
            }
          }
        }
        else{
          if(manager){
            if(manager->findNode(this->name())){
              WARNING("Borra el evento");
              event=manager->getEvent(this->name());
              manager->removeEvent(this->name());
            }
          }
        }
              
        DEBUG("luaevent nodeon requested: %d", nodeon);
        return;
      }
      else if(paramName== "noderepeat"){
        noderepeat = object2int(value);
        DEBUG("luaevent noderepeat requested: %d", noderepeat);
        return;
      } 
    else
    return Node::setAttr(paramName,value);
}

bool LUAEvent::check()
{
  try {
    cont++;
    if ( std::find(checkCode.begin(), checkCode.end(), '=') != checkCode.end() )
      return object2int(manager->eval(checkCode,name())) != 0;
    else
      return object2int(manager->eval(std::string("print(")+checkCode+")",name())) != 0;
  } catch(const std::exception &e) {
    WARNING("Catch exception calling LUA code: %s",e.what());
  }
  return false;
}

bool LUAEvent::sync()
{
  try {
    if ( std::find(syncCode.begin(), syncCode.end(), '=') != syncCode.end() )
      return object2int(manager->eval(syncCode,name())) != 0;
    else
      return object2int(manager->eval(std::string("print(")+syncCode+")",name())) != 0;
  } catch(const std::exception &e) {
    WARNING("Catch exception calling LUA code: %s",e.what());
  }
  return false;
}
