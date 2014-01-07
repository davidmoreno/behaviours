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

#include "connection.h"
#include "manager.h"
#include "node.h"
#include "action.h"
#include "log.h"

using namespace AB;

/**
 * @short Creates a random name, so that new obejcts have a valid name always.
 */
static std::string randomName(const std::string &base)
{
  static int n=0;
  n++;
  char tmp[17];
  snprintf(tmp, sizeof(tmp), "%016d", n);
  return base + tmp;
}

Connection::Connection(Manager *manager_, Node::p from, Node::p to) : manager(manager_), from_(from), to_(to)
{
  DEBUG("Connected %s to %s", from_->name().c_str(), to_->name().c_str());
  name_=randomName("c_");
}

bool Connection::setGuard(const std::string &guard)
{
  guard_=guard;
  DEBUG("Set guard for %s", unicode().c_str());
  return true;
}

bool Connection::checkGuard()
{
  DEBUG("Check if exec %s", unicode().c_str());
  if (guard_=="")
    return false;
  try {
    return manager->check(guard_);
  } catch(const std::exception &e) {
    WARNING("Error parsing the guard [%s]: %s", guard_.c_str(), e.what());
    return false;
  }
}


std::string Connection::unicode()
{
  if (guard_.empty())
    return from_->name()+"->"+to_->name();
  else
    return from_->name()+"->"+to_->name()+" ["+guard_+"]";
}
