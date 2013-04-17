/*
 * (C) 2013 Coralbits SL & AISoy Robotics.
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "luaaction.h"
#include "../manager.h"

using namespace AB;
using namespace std;

LUAAction::LUAAction(const char* type) : Action(type) 
{
  
}

void LUAAction::setManager(Manager* m)
{
  manager=m;
}

void LUAAction::setAttr(const std::string &k, AB::Object v)
{
  if (k=="exec" || k=="code")
    code=object2string(v);
  else if (k=="flags")
    setFlags(object2int(v));
  else
    return Node::setAttr(k,v);
}

Object LUAAction::attr(const std::string &k)
{
  if (k=="exec" || k=="code")
    return to_object(code);
  if (k=="flags")
    return to_object(flags());
  return Node::attr(k);
}

AttrList LUAAction::attrList()
{
  AttrList p=Node::attrList();
  p.push_back("code");
  p.push_back("flags");
  return p;
}


void LUAAction::exec()
{
  try {
    if ( std::find(code.begin(), code.end(), '=') != code.end() )
      manager->eval(code,name());
    else
      manager->eval(std::string("print(")+code+")",name());    
  } catch(std::exception &e) {
    ERROR("Catch exception at %s calling LUA code: %s",name().c_str(),e.what());
  }
}
