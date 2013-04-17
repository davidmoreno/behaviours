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

#ifndef __LUAACTION_H__
#define __LUAACTION_H__

#include "../action.h"

namespace AB {
  class LUAAction : public Action {
  public:
    LUAAction(const char* type = "LUAAction");
    void exec();
    void setAttr(const std::string &k, AB::Object v);
    Object attr(const std::string &k);
    AttrList attrList();
    void setManager(Manager *);
  private:
    Manager *manager;
    std::string code;
  };
}

#endif
