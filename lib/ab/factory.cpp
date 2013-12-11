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

#include "factory.h"
#include "manager.h"
#include "action.h"
#include "event.h"
#include "log.h"

using namespace AB;
std::map<std::string, std::function<Node::p(const std::string& type)> > Factory::knownTypes;

bool Factory::registerCreator(const std::string &type, std::function<Node::p(const std::string& type)> f)
{
  knownTypes[type]=f;
  return true;
}

Node::p Factory::createNode(const std::string& type)
{
  if (knownTypes.count(type)>0) {
    return knownTypes[type](type);
  }
  throw type_does_not_exists(type);
}


Event::p Factory::createEvent(const std::string& type)
{
  Event::p n=std::dynamic_pointer_cast<Event>(createNode(type));
	if (n)
		return n;
  throw type_does_not_exists(type);
}

Action::p Factory::createAction(const std::string& type)
{
  Action::p n=std::dynamic_pointer_cast<Action>(createNode(type));
	if (n)
		return n;
  throw type_does_not_exists(type);
}

std::string Factory::list_as_string()
{
  std::string ret;
  std::map<std::string, std::function<Node::p(const std::string& type)> >::iterator I=Factory::knownTypes.begin(), endI=Factory::knownTypes.end();
  for(; I!=endI; ++I) {
    ret+="<"+I->first+"> ";
  }
  return ret;
}

std::vector<std::string> Factory::list(){
	std::vector<std::string> ret;
  std::map<std::string, std::function<Node::p(const std::string& type)> >::iterator I=Factory::knownTypes.begin(), endI=Factory::knownTypes.end();
  for(; I!=endI; ++I){
    ret.push_back(I->first);
  }
  return ret;
}
