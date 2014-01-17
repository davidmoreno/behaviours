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

#include "events/start.h"
#include "events/timeout.h"
#include "events/luaevent.h"
#include "events/alarm.h"
#include "events/external.h"

#include "actions/luaaction.h"
#include "actions/empty.h"
#include "actions/webservice.h"
#include "actions/load.h"
#include "actions/wait.h"
#include "actions/stop.h"

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

