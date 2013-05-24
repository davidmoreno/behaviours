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

#ifndef DIA_WEBSERVICE_H
#define DIA_WEBSERVICE_H

#include <string>

#include <ab/action.h>

namespace AB {

  class WebService : public AB::Action {
  public:
    WebService(const char * type ="webservice");

    virtual void setAttr(const std::string& name, AB::Object obj);
    virtual AB::Object attr(const std::string& name);
    virtual AB::AttrList attrList();

    void setManager(AB::Manager* manager);
    void exec();
  private:
    std::string service;
    std::string url;
    int timeout;
    AB::Manager *manager;
  };
}

#endif
