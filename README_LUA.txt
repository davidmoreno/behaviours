Introduction
------------

AB nodes can be programmed using LUA. These nodes can do all that C++ code can do, but as they are interpreted, they can just be used at runtime.

There is a LUAAction and a LUAEvent, each should be used as needed.

There is also an special object, 'manager', that allows some manager interaction:

manager.cancel(). Stops exection of current AB.
manager.notify(nodename) | manager.notify(node). Notifies that node.

Access to known nodes can be made by name or by type. If by type the last node is checked first and if not, returns any matching on current set.

On the returned table you have all exported parameters and two extra: name and type. An example of LUA code mught be:

  event:

  manager.notify("LUAAction");

  action:

  manager.cancel();

User can create global variables and access them from any node.


Creating LUA object in C++ code
-------------------------------


To create a LUA object from a C++ AB::Object, you must reimplement the a LUAObject object, and the to_lua method. If this object is
returned to LUA, to_lua is called automatically each time lua access such atribute.

As this can be too much work for each attribute, there are helper functions at luaobject.h:

  lua_bridge(method, this);
  lua_bridge_to_function(function);

both construct AB::Object that will call the desired method or function 'int to_lua(lua_State *s)'. Check lua tests for complex examples, 
or DIA/action/image.cpp for a simple AISoy1::Image to luacv images.


