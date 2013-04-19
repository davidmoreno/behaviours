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

#include <ab/log.h>

#include "stop.h"
#include <ab/manager.h>

using namespace AB;

Stop::Stop(const char *type) : Action(type)
{
}

void Stop::exec()
{
 if(manager) {
   WARNING("Stopping");
   manager->cancel();
 } else {
   ERROR("Unable to stop: no Manager available");
 }
}

void Stop::setManager(AB::Manager* m)
{
  manager=m;
}
