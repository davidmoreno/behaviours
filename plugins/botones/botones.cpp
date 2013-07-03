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

#include "botones.hpp"
#include <ab/plugin.hpp>
#include <ab/factory.h>
#include <ab/log.h>

void ab_init(void){
	DEBUG("Loaded ab init at basic example");
	AB::Factory::registerClass<Botones>("botones");
}

Botones::Botones(const char* type): Action(type)
{

}

void Botones::exec()
{
	INFO("Exec botones %p",this);
}
