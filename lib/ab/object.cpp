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

#define _BSD_SOURCE
#include <stdio.h>
#include <sstream>

#include "log.h"
#include "object.h"
#include "object_basic.h"

#include <stdlib.h>

using namespace AB;

const AttrList ObjectBase::emptyAttrList;
int64_t ObjectBase::live_objects=0;

namespace std{
	std::string to_string(const AB::ObjectBase *p){
		std::stringstream ss;
		ss<<"<Object "<<p->type()<<" @"<<p<<">";
		return ss.str();
	}
};

ObjectBase::ObjectBase(const char* type)  : _type(type)
{
  ObjectBase::live_objects++;
  DEBUG("%s() %s (%lld live)",type, std::to_string(this).c_str(), ObjectBase::live_objects);
}

ObjectBase::~ObjectBase()
{
  ObjectBase::live_objects--;
  DEBUG("~%s() %s (%lld still alive)",type(), std::to_string(this).c_str(), ObjectBase::live_objects);
}


namespace AB {
  const char *String::type="String";
  const char *Integer::type="Integer";
  const char *Float::type="Float";
  const char *CallableObject::type="Callable";

  Object to_object(const std::string &s)
  {
    return std::make_shared<String>(s);
  }
  Object to_object(int n)
  {
    return std::make_shared<Integer>(n);
  }
  Object to_object(double n)
  {
    return std::make_shared<Float>(n);
  }
  Object to_object(std::function<Object(ObjectList&)> f)
  {
    return std::make_shared<CallableObject>(f);
  }

  std::string object2string(const Object &obj)
  {
    if (obj->type()==String::type) {
      return static_cast<String*>(obj.get())->s;
    } else if (obj->type()==Integer::type) {
      std::stringstream ss;
      ss<<static_cast<Integer*>(obj.get())->n;
      return ss.str();
    } else if (obj->type()==Float::type) {
      std::stringstream ss;
      ss<<static_cast<Float*>(obj.get())->n;
      return ss.str();
    }
    try {
      return object2string( obj->attr("__str__")->call() );
    } catch(const attribute_not_found &e) {
      //throw(object_not_convertible("String"));
    }
    throw(object_not_convertible(obj->type(),"string"));
  }

  int object2int(const Object &obj)
  {
    if (obj->type()==Integer::type) {
      return static_cast<Integer*>(obj.get())->n;
    } else if (obj->type()==Float::type) {
      return static_cast<Float*>(obj.get())->n;
    } else if (obj->type()==String::type) {
      return atoi(static_cast<String*>(obj.get())->s.c_str());
    }
    try {
      return object2int( obj->attr("__int__")->call() );
    } catch(const attribute_not_found &e) {
      //throw(object_not_convertible("String"));
    }
    throw(object_not_convertible(obj->type(),"int"));
  }
  double object2float(const Object &obj)
  {
    if (obj->type()==Integer::type) {
      return static_cast<Integer*>(obj.get())->n;
    } else if (obj->type()==Float::type) {
      return static_cast<Float*>(obj.get())->n;
    } else if (obj->type()==String::type) {
      return atof(static_cast<String*>(obj.get())->s.c_str());
    }
    try {
      return object2float( obj->attr("__float__")->call() );
    } catch(const attribute_not_found &e) {
      //throw(object_not_convertible("String"));
    }
    throw(object_not_convertible(obj->type(),"float"));
  }
}


std::string std::to_string(const Object &p)
{
	std::stringstream ss;
	ss<<"<Object "<<p->type()<<" @"<<p.get()<<">";
	return ss.str();
}

