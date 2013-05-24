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

#ifndef AB_LUA_H
#define AB_LUA_H

#include <string>
#include "object.h"

typedef struct lua_State lua_State;

namespace AB {
  class Manager;
  /// Set this pointer to another function to overwrite the print.
  extern void (*lua_ab_print_real)(const std::string &str);

  /**
   * @short Allows to bridge custom LUA data types to C++ code.
   *
   * This objects are gotten at the C++ lua methods (see method2object), and should be
   * dynamic_casted and used there; index may be invalid after return so should not be stored.
   */
  class LUAData : public ObjectBase {
  public:
    static const char *type;
    lua_State *state;
    int index;

    LUAData(lua_State *state, int index) : ObjectBase(LUAData::type), state(state), index(index) {}

    static LUAData fromObject(Object &o) {
      if (o->type()!=LUAData::type) {
        throw AB::object_not_convertible(o->type(), LUAData::type);
      }
      LUAData *r=static_cast<LUAData*>(o.get());
      return *r;
    }
  };

  /**
   * @short Lua interpreter. It can run source files, strings and simple expressions.
   *
   * It communicates with the manager to get the proper objects when needed.
   *
   */
  class LUA {
    lua_State *state;
  public:
    /// This value is unique and can be used to find the manager at the LUA_REGISTRYINDEX.
    static const char *MANAGER_UUID;

    class exception : public std::exception {
      std::string str;
    public:
      exception(lua_State *state);
      exception(const std::string &s) : str(s) {
        lua_ab_print_real("EXCEPTION: "+str);
      };
      ~exception() throw() {};
      virtual const char* what() const throw() {
        return str.c_str();
      }
    };

    LUA();
    ~LUA();

    Object eval(const std::string &code, const std::string &name="=inline_code");
    bool check(const std::string &expression, const std::string &name="=inline_code");

    void setManager(Manager *);
    /**
     * @short Direct access to the lua_State.
     *
     * This breaks encapsulation, but may be needed for deep startup manipulation
     * of LUA executing environment.
     */
    lua_State *getState() { return state; }
  };

  int lua_ab_showstack(lua_State *state, int start);
}

#endif
