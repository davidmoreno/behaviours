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

#ifndef LOAD_H
#define LOAD_H

#include <ab/action.h>

namespace AB {
  class Load : public AB::Action {
  public:
    Load(const char * type = "load");

    virtual void setAttr(const std::string& name, AB::Object obj);
    virtual AB::Object attr(const std::string& name);
    virtual AB::AttrList attrList();
    void setManager(AB::Manager* manager);
    void exec();
  private:
    std::string fileToLoad;
    AB::Manager *manager;
  };
}

#endif
