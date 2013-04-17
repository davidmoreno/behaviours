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

#include <sstream>

#include "log.h"
#include "lua.h"
#include "manager.h"
#include "object.h"
#include "node.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
using namespace AB;


const char *LUAData::type="lua_object";
const char *LUA::MANAGER_UUID = "82ae3537-f330-40fd-a218-e0a088bc8c16";

/// C Style static functions defined here.
#include "lua_helpers.cpp"

LUA::LUA()
{
  state = luaL_newstate();
  if (!state) {
    throw(LUA::exception(state));
  }

  luaL_openlibs(state);
  lua_settop(state, 0);

  /// Set the metatable for the global environment, with function __index that search for unknown vars.
	lua_rawgeti(state, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	
  lua_newtable(state);
  lua_pushstring(state, "__index");
  lua_pushcfunction(state, lua_G_index);
  lua_settable(state, -3);
	
  lua_setmetatable(state, -2);
	lua_pop(state,1);
	
	// Some other global known elements
  lua_pushcfunction(state, lua_ab_print);
  lua_setglobal(state,"print");

  lua_pushcfunction(state, lua_ab_dir);
  lua_setglobal(state,"dir");

  lua_pushcfunction(state, lua_ab_tostring);
  lua_setglobal(state,"tostring");

  luaL_newmetatable(state, "AB.userobject");
  lua_pushstring(state, "__gc");
  lua_pushcfunction(state, lua_ab_free_userobject);
  lua_settable(state, -3);


}

LUA::~LUA()
{
  lua_close(state);
}


void LUA::setManager(Manager *m)
{
  lua_pushstring(state, MANAGER_UUID);
  lua_pushlightuserdata(state, m);
  lua_settable(state, LUA_REGISTRYINDEX);

  /// Add the manager object, maybe replace it
  lua_newtable(state);
  lua_pushstring(state, "cancel");
  lua_pushcfunction(state, lua_cancel_manager);
  lua_settable(state,-3);
  lua_pushstring(state, "notify");
  lua_pushcfunction(state, lua_notify_manager);
  lua_settable(state,-3);
  lua_setglobal(state, "manager");

  /// Add the metatable for Nodes
  lua_newtable(state);
  lua_pushstring(state,"__index");
  lua_pushcfunction(state, lua_node_index);
  lua_settable(state,-3);
  lua_pushstring(state,"__newindex");
  lua_pushcfunction(state, lua_node_newindex);
  lua_settable(state,-3);
  lua_setglobal(state, "__node_metatable");

  DEBUG("Set manager at %p",m);
}

typedef struct LoadString_Aux {
  const char *text[8];
  size_t size[8];
  int i;
} LoadS;

/// Auxiliary to load data into lua stack
static const char *lua_ab_load_string (lua_State *L, void *data, size_t *size)
{
  LoadString_Aux *S=(LoadS*) data;
  int i=S->i++;
  *size=S->size[i];
  (void)L;
  return S->text[i];
}

Object LUA::eval(const std::string& code, const std::string &name)
{
  DEBUG("Evaluating at %s:\n%s", name.c_str(), code.c_str());
  LoadString_Aux S;
  S.text[0]=code.c_str(); S.size[0]=code.length();
  S.text[1]=NULL;   S.size[1]=0;
  S.i=0;
  int error=lua_load(state,lua_ab_load_string,&S,name.c_str(),NULL);
  if (error) {
    throw(LUA::exception(state));
  };
  lua_pushvalue(state,-1);
  error=lua_pcall(state, 0,1,0);
  if (error) {
    throw(LUA::exception(lua_tostring(state,-1)));
  }
  if (lua_isstring(state,-1))
    return to_object(lua_tostring(state,-1));
  if (lua_isnumber(state,-1))
    return to_object(lua_tonumber(state,-1));
  if (lua_isboolean(state,-1)) {
    bool a=lua_toboolean(state,-1);
    //DEBUG("Boolean is %d",a);
    return to_object(a);
  }
  DEBUG("No return value, top %d, type %d", lua_gettop(state), lua_type(state,-1));
  return to_object(0);
}

bool LUA::check(const std::string& expression, const std::string &name)
{
  DEBUG("Evaluating at %s: %s", name.c_str(), expression.c_str());

  LoadS S;
  S.text[0]="return "; S.size[0]=sizeof("return ")-1;
  S.text[1]=expression.c_str(); S.size[1]=expression.length();
  S.text[2]=NULL;   S.size[2]=0;
  S.i=0;
  int error=lua_load(state,lua_ab_load_string,&S,name.c_str(), NULL);
  if (error) {
    throw(LUA::exception(state));
  };
  lua_pushvalue(state,-1);
  error=lua_pcall(state,0,1,0) || !lua_isboolean(state, -1);
  if (error) {
    std::stringstream ss;
    if (lua_isstring(state, -1)) {
      ss<<"Error parsing: "<<lua_tostring(state, -1);
    } else {
      ss<<"Result is not boolean, it is type "<<lua_type(state, -1)<<" (boolean is type "<<LUA_TBOOLEAN<<")";
    }
    throw(LUA::exception(ss.str()));
  } else
    return lua_toboolean(state, -1)!=0;
}

LUA::exception::exception(lua_State *state)
{
  str=lua_tostring(state, -1);
  lua_ab_print_real("EXCEPTION: "+str);
}

