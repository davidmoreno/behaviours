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

#include <iostream>

#include "log.h"
#include "lua.h"
#include "luaobject.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "node.h"
#include "manager.h"
#include "object_basic.h"
#include <iosfwd>
}
using namespace AB;


const char *AB::LUAObject::type="LUAObject";


class PushVisitor;
static int lua_ab_pushobject(lua_State *state, AB::Object o);
static AB::Object lua_ab_toobject(lua_State *State, int index); // Transforms anything, int, string...
AB::Object lua_ab_to_userobject(lua_State *state, int index); // At given pos there is a ptr of a object

static int lua_cancel_manager(lua_State *state)
{
  lua_pushstring(state, LUA::MANAGER_UUID);
  lua_gettable(state, LUA_REGISTRYINDEX);
  Manager *m=(Manager*)lua_touserdata(state,-1);
  m->cancel();
  return 0;
}

static int lua_notify_manager(lua_State *state)
{
  const char *nodename;
  if (lua_isstring(state,1))
    nodename=luaL_checkstring(state,1);
  else {
    lua_pushstring(state,"name");
    lua_gettable(state,-2);
    nodename=luaL_checkstring(state,-1);
  }
  lua_pushstring(state, LUA::MANAGER_UUID);
  lua_gettable(state, LUA_REGISTRYINDEX);
  Manager *m=(Manager*)lua_touserdata(state,-1);

  Node *n=m->getNode(nodename);
  m->notify(n);

  return 0;
}

static int lua_G_index(lua_State *state)
{
  const char *var=luaL_checkstring(state, -1);
  DEBUG("Get var %s", var);
  lua_pushstring(state, LUA::MANAGER_UUID);
  lua_gettable(state, LUA_REGISTRYINDEX);
  Manager *m=(Manager*)lua_touserdata(state,-1);
  DEBUG("Got manager at %p",m);
  if (!m) {
    throw(LUA::exception("Could not find manager object"));
  }
  lua_ab_pushobject(state,m->resolve(var));
  return 1;
}

static int lua_ab_call_method(lua_State *state)
{
  Object node=lua_ab_to_userobject(state, lua_upvalueindex(1));
  DEBUG("Calling object %p",node.get());
  ObjectList ol;
  int i=1,endi=lua_gettop(state);
  //DEBUG("Push OL");
  for(; i<=endi; ++i) {
    ol.push_back(lua_ab_toobject(state, i));
  }
  //DEBUG("Done ol. type %s",node->type());

  Object o=node->call(ol);

  int r=lua_ab_pushobject(state,o);

  DEBUG("Done");
  return r;
}
/*
static int lua_ab_push_method(lua_State *state, const std::string &methodname, Node *node)
{
  DEBUG("Adding LUA method %s to stack",methodname.c_str());

  lua_pushstring(state,methodname.c_str());
  lua_pushlightuserdata(state,node);
  lua_pushcclosure(state, lua_ab_call_method, 2);

  return 1;
}
*/
static int lua_ab_push_param_list(lua_State *state, Object node)
{
  lua_newtable(state);
  Node *real_node=dynamic_cast<Node*>(node.get());
  if (real_node) {
    lua_pushstring(state,"name");
    lua_pushstring(state,real_node->name().c_str());
    lua_settable(state,-3);
  }
  lua_pushstring(state,"type");
  lua_pushstring(state,node->type());
  lua_settable(state,-3);
  lua_pushstring(state,"param_list");
  lua_pushstring(state,"self");
  lua_settable(state,-3);

  {
    // List of attrs
    AB::AttrList l(node->attrList());
    AB::AttrList::iterator I=l.begin(), endI=l.end();
    for(; I!=endI; ++I) {
      lua_pushstring(state, I->c_str());
      lua_ab_pushobject(state, node->attr(*I));
      lua_settable(state,-3);
    }
  }
  /*
  { // List of methods
    AB::Node::MethodDict m(node->methodDict());
    AB::Node::MethodDict::iterator I=m.begin(), endI=m.end();
    for(; I!=endI; ++I) {
      lua_ab_push_method(state, I->first, node);
    }
  }
  */
  return 1;
}

static int lua_node_index(lua_State *state)
{
  const char *k=luaL_checkstring(state,2);
  lua_pushstring(state,"_ptr");
  lua_gettable(state,1);
  AB::Object obj=lua_ab_to_userobject(state,-1);
  DEBUG("Access object %p",obj.get());
  try {
    return lua_ab_pushobject(state, obj->attr(k));
  } catch(const AB::attribute_not_found &e) {}

  if (strcmp(k,"param_list")==0) {
    return lua_ab_push_param_list(state, obj);
  }
  /*  else {
      DEBUG("Test if its a method: %s", k);
      AB::Node::MethodDict m(node->methodDict());
      if (m.count(k)>0){
        return lua_ab_push_method(state, k, node);
      }
    }
    */
  throw (AB::attribute_not_found(k));
}

static int lua_node_newindex(lua_State *state)
{
  const char *k=luaL_checkstring(state,2);
  lua_pushstring(state,"_ptr");
  lua_gettable(state,1);
  AB::Object obj=lua_ab_to_userobject(state,-1);
  obj->setAttr(k, lua_ab_toobject(state,3));
  return 1;
}

/**
 * @short Pushes a userdata that will use normal shared_ptr semantics, but usable from LUA.
 */
int lua_ab_push_userobject(lua_State *s, AB::Object &obj)
{
  AB::Object **o=(Object**)lua_newuserdata(s,sizeof(Object**));
  *o=new Object;
  **o=obj;

  luaL_getmetatable(s, "AB.userobject");
  lua_setmetatable(s, -2);

  DEBUG("Push %s:%p at pointer %p, count is %d",obj->type(),obj.get(), o, (int)obj.use_count());
  return 1;
}

AB::Object lua_ab_to_userobject(lua_State *state, int index)
{
  DEBUG("lua_ab_to_userobject: got %d",lua_type(state,index));
#ifndef __arm__
  Object **o=(AB::Object**)luaL_checkudata(state,index,"AB.userobject");
#else
  Object **o=(AB::Object**)lua_touserdata(state,index); // On ARM there is a BUG that luaL does not work properly ¿?
#endif
  if (!o | !(*o))
    throw(LUA::exception("Cant access object userdata."));
  DEBUG("Done");
  return **o;
}

int lua_ab_free_userobject(lua_State *state)
{
  DEBUG("lua_ab_free_userobject: got %s",luaL_typename(state,1));;
  Object **o=(AB::Object**)luaL_checkudata(state,1,"AB.userobject");
  if (!o | !(*o))
    throw(LUA::exception("Cant access object userdata."));
  //DEBUG("Free object from LUA: %s, count is %d",(**o)->type(),(int)(**o).use_count());
  delete *o; // This may lead to shared_ptr refcount--==0, and free the underlying object.
  return 0;
}

static int lua_ab_pushobject(lua_State *state, AB::Object o)
{
  const char *t=o->type();
  if (t==AB::String::type) {
    lua_pushstring(state, object2string(o).c_str());
  } else if (t==AB::Integer::type) {
    lua_pushnumber(state, object2int(o));
  } else if (t==AB::Float::type) {
    lua_pushnumber(state, object2float(o));
  } else if (t==AB::CallableObject::type) {
    lua_ab_push_userobject(state, o);
    lua_pushcclosure(state, lua_ab_call_method, 1);
  } else if (t==AB::LUAObject::type) {
    LUAObject *luaobj=dynamic_cast<LUAObject*>(o.get());
    return luaobj->to_lua(state);
  } else {
    AB::AttrList l=o->attrList();
    lua_createtable(state, 0, l.size());

    Node *n=dynamic_cast<Node*>(o.get());
    if (n) {
      lua_pushstring(state, "name");
      lua_pushstring(state, n->name().c_str());
      lua_settable(state,-3);
    }

    lua_pushstring(state, "type");
    lua_pushstring(state, o->type());
    lua_settable(state,-3);

    lua_pushstring(state, "_ptr");
    lua_ab_push_userobject(state,o);
    lua_settable(state,-3);

    lua_getglobal(state, "__node_metatable");
    lua_setmetatable(state, -2);
  }

  return 1;
}

static AB::Object lua_ab_toobject(lua_State *state, int index)
{
  switch(lua_type(state, index)) {
  case LUA_TNUMBER:
    return to_object(lua_tonumber(state, index));
  case LUA_TBOOLEAN:
    return to_object(lua_toboolean(state, index));
  case LUA_TSTRING:
    return to_object(lua_tolstring(state, index, NULL));
  case LUA_TUSERDATA:
    return AB::Object(new LUAData(state,index));
  default:
    ;
  }
  throw(LUA::exception(std::string("Cant convert LUA ")+luaL_typename(state,index)+" to an AB element"));
}

static int lua_ab_dir(lua_State *state);


static std::string lua_ab_tostring_i(lua_State *state, int index=-1,bool quote=false)
{
  std::stringstream ss;
  switch(lua_type(state, index)) {
  case LUA_TNUMBER:
    ss<<lua_tonumber(state, index);
    break;
  case LUA_TBOOLEAN:
    if (lua_toboolean(state, index))
      ss<<"true";
    else
      ss<<"false";
    break;
  case LUA_TSTRING:
    if (quote)
      ss<<"'"<<lua_tolstring(state, index,NULL)<<"'";
    else
      ss<<lua_tolstring(state, index,NULL);
    break;
  case LUA_TNIL:
    ss<<"nil";
    break;
  case LUA_TFUNCTION:
    ss<<"function";
    break;
  case LUA_TTABLE: {
    lua_ab_dir(state);
    ss<<"table { ";
    lua_pushnil(state);
    bool first=true;
    while(lua_next(state, index-1)) {
      if (first)
        first=false;
      else
        ss<<", ";
      lua_pop(state, 1);
      ss<<lua_ab_tostring_i(state,-1,true);
    }
    ss<<" }";
  }
  break;
  default:
    ss<<"**unknown**";
  }
  return ss.str();
}

static int lua_ab_tostring(lua_State *state)
{
  lua_pushstring(state, lua_ab_tostring_i(state,-1).c_str());
  return 1;
}

static void lua_ab_print_real_default(const std::string &str)
{
  INFO("LUA: %s",str.c_str());
}

void (*AB::lua_ab_print_real)(const std::string &str) = lua_ab_print_real_default;

static int lua_ab_print(lua_State *state)
{
  std::string str=lua_ab_tostring_i(state);
	lua_pushstring(state, LUA::MANAGER_UUID);
	lua_gettable(state, LUA_REGISTRYINDEX);
	Manager *m=(Manager*)lua_touserdata(state,-1);
	
	m->eventQueue.pushEvent("lua_console", "message", str);
  return 0;
}

namespace AB {
  int lua_ab_showstack(lua_State *state, int start)
  {
    int end=lua_gettop(state);
    INFO("Stack from %d to %d",start,end);
    for(int i=start; i<=end; i++) {
      std::string val;
      try {
        std::stringstream ss;
        ss<<lua_typename(state,lua_type(state,i))<<" "<<lua_type(state,i);
        val=ss.str(); //lua_typename(state,i);//lua_ab_tostring_i(state, i);
      } catch(LUA::exception &e) {
        val=std::string("Exception catched: ")+e.what();
      }

      INFO("%2d: %s",i, val.c_str());
    }
    return 1;
  }
}

static int lua_ab_dir(lua_State *state)
{
  int top=lua_gettop(state);

  DEBUG("%d",top);
  if (top==0) {
    lua_pushstring(state, LUA::MANAGER_UUID);
    lua_gettable(state, LUA_REGISTRYINDEX);
    Manager *m=(Manager*)lua_touserdata(state,-1);

    lua_pushstring(state,"_G");
		lua_getglobal(state, "_G"); 
    lua_ab_dir(state); // dir(_G)


    std::set<Node*> nodes=m->getNodes();
    std::set<Node*>::iterator I=nodes.begin(), endI=nodes.end();

    int i;
    for(i=0; I!=endI; ++I,++i) {
      lua_pushstring(state,(*I)->name().c_str());
      lua_pushboolean(state,true);
      lua_settable(state,-3);
    }
  } else if (lua_type(state,top)==LUA_TTABLE) {
    bool has_ptr=false;
    DEBUG("Get vars of a table");
    std::vector<std::string> v;
    lua_pushnil(state);
    while(lua_next(state, top)) {
      lua_pop(state,1);
      std::string str=lua_ab_tostring_i(state);
      if (str=="_ptr")
        has_ptr=true;
      else // Do not expose _ptr.
        v.push_back(str);
    }
    if (has_ptr) { // Its an AB::Object, I add all paramList.
      lua_pushstring(state,"_ptr");
      lua_gettable(state,top);

      AB::Object obj=lua_ab_to_userobject (state,-1);
      lua_pop(state,1);
      AttrList l=obj->attrList();
      AttrList::iterator I=l.begin(),endI=l.end();
      for(; I!=endI; ++I) {
        DEBUG("Push %s",I->c_str());
        v.push_back( *I );
      }
    }

    std::vector<std::string>::iterator I=v.begin(),endI=v.end();
    lua_newtable(state);
    for(; I!=endI; ++I) {
      lua_pushstring(state, I->c_str());
      lua_pushboolean(state,true);
      lua_settable(state,-3);
    }


  } else {
    lua_newtable(state);
    WARNING("Trying to get a dir of a unmanaged type of object");
  }

  return 1;
}

