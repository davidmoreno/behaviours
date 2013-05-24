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

#ifndef AB_LUA_OBJECT_H
#define AB_LUA_OBJECT_H

#include <functional>
#include <memory>

#include "object.h"
extern "C" {
  struct lua_State;
}
namespace AB {
  class LUAObject : public ObjectBase {
  public:
    static const char *type;
    LUAObject() : ObjectBase(LUAObject::type) {};

    virtual int to_lua(lua_State *s) = 0;
  };

  class lua_bridge_class : public AB::LUAObject {
    typedef std::function<int (lua_State *)> f_t;
    f_t f;
  public:
    lua_bridge_class(f_t _f) {
      f=_f;
    }
    int to_lua(lua_State *s) {
      return f(s);
    }
  };

  template<typename T>
  Object lua_bridge(int (T::*m)(lua_State *), T *o)
  {
    return std::make_shared<lua_bridge_class>(std::bind(m,o,std::placeholders::_1));
  }

  static inline Object lua_bridge_to_function(int (*f)(lua_State *))
  {
    return std::make_shared<lua_bridge_class>(f);
  }

}


#endif
