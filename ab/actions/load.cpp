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

#include <ab/manager.h>
#include "load.h"

using namespace AB;

Load::Load(const char * type) : Action(type) 
{
  
}

void Load::setAttr(const std::string& name, AB::Object obj)
{
  fileToLoad=object2string(obj);
}

AB::Object Load::attr(const std::string& name)
{
  return to_object(fileToLoad);
}

AttrList Load::attrList()
{
  AttrList l;
  l.push_back("filename");
  return l;
}



void Load::setManager(Manager* manager_)
{
  manager=manager_;
}

void Load::exec()
{
  if (fileToLoad.empty())
    return;
  manager->clear();
  manager->loadBehaviour("html/"+fileToLoad);
}
