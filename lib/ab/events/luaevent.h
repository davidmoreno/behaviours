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

#ifndef __LUA_EVENT_H__
#define __LUA_EVENT_H__

#include "../event.h"

namespace AB {
  class LUAEvent : public Event {
  public:
    LUAEvent(const char* type = "lua_event");
    Object attr(const std::string& paramName);
    void setAttr(const std::string& paramName, AB::Object value);
    AttrList attrList();
    void setManager(Manager* manager);
    bool check();
    bool sync();
  private:
    Manager *manager;
    std::string checkCode;
    std::string syncCode;
    Event::p event;
  };
}

#endif
