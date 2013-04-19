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


#include "builtin_nodes.hpp"

#include <ab/events/start.h>
#include <ab/events/timeout.h>
#include <ab/events/luaevent.h>
#include <ab/events/alarm.h>
#include <ab/events/external.h>

#include <ab/actions/luaaction.h>
#include <ab/actions/empty.h>
#include <ab/actions/webservice.h>
#include <ab/actions/load.h>
#include <ab/actions/wait.h>
#include <ab/actions/stop.h>

#include "factory.h"

void AB::registerBuiltinNodes()
{
  AB::Factory::registerClass<AB::Start>("start");
  AB::Factory::registerClass<AB::Timeout>("timeout");
  AB::Factory::registerClass<AB::LUAEvent>("lua_event");
  AB::Factory::registerClass<AB::LUAAction>("lua_action");
  AB::Factory::registerClass<AB::Empty>("condmerge");
	
  AB::Factory::registerClass<AB::Alarm>("alarm");
  AB::Factory::registerClass<AB::External>("click");
  AB::Factory::registerClass<AB::Load>("load");
  AB::Factory::registerClass<AB::Wait>("wait");
  AB::Factory::registerClass<AB::WebService>("webservice");
  AB::Factory::registerClass<AB::Stop>("stop");
}

