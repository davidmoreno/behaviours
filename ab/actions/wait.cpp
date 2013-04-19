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
#include <algorithm>
#include <aisoy1/bot.h>
#include "wait.h"

using namespace AB;

extern AISoy1::Bot bot;

Wait::Wait(const char *type) : AB::Action(type)
{
  t=0;
}

void Wait::setAttr(const std::string &k, AB::Object s)
{
  t=object2float(s);
}

AB::Object Wait::attr(const std::string &k)
{
  return to_object(t);
}

AB::AttrList Wait::attrList()
{
  AB::AttrList l;
  l.push_back("t");
  return l;
}

void Wait::exec()
{
  DEBUG("Wait %f s",t);
  usleep(t*1000000.0);
}
