#include <ab/log.h>
#include "../tests.h"

#include <algorithm>

#include <ab/lua.h>
#include <ab/manager.h>
#include <iostream>
#include <ab/node.h>
#include <ab/events/timeout.h>


using namespace AB;
using namespace std;

template<typename T>
bool check(T &mgr, const char *expr)
{
  bool ret=mgr.check(expr);
  INFO("Expression <%s> => %s", expr, ret ? "true" : "false");
  return ret;
}

void t00_resolve()
{
  INIT_LOCAL();

  Manager manager;
  Node::p n=std::make_shared<Timeout>();
  n->setName("tout");
  n->setAttr("timeout","9.0");
  manager.addNode(n);

  cout<<object2string(to_object(n))<<endl;

  FAIL_IF_NOT_EXCEPTION(manager.resolve("should_not_exist"));
  FAIL_IF_NOT_EXCEPTION(manager.resolve("getvar"));
  FAIL_IF_EXCEPTION(manager.resolve("tout"));

  cout<<object2string(manager.resolve("tout"))<<endl;

  FAIL_IF_NOT_EQUAL_STRING(object2node(manager.resolve("tout"))->name(), "tout");

  FAIL_IF_NOT_EXCEPTION(object2int(manager.resolve("tout")));
  FAIL_IF_NOT_EQUAL_INT(object2int(manager.resolve("tout")->attr("timeout")),9);
  FAIL_IF_NOT_EQUAL_INT(object2int(manager.resolve("count")),0);

  END_LOCAL();
}

void t01_simple()
{
  INIT_LOCAL();

  LUA l;

  l.eval("print('Hola mundo');"); SUCCESS();
  cout <<boolalpha;
  FAIL_IF_NOT( check(l, "true") );
  FAIL_IF_NOT( check(l, "true and true") );
  FAIL_IF_NOT( check(l, "10>5") );
  FAIL_IF( check(l, "5>10") );

  FAIL_IF_NOT_EXCEPTION( check(l, "no 11") );

  END_LOCAL();
}

void t02_with_manager()
{
  INIT_LOCAL();
  Manager manager;

  manager.eval("print(_VERSION)");

  Node::p n=std::make_shared<Timeout>();
  FAIL_IF_EXCEPTION( n->setAttr("timeout","9") );
  n->setName("tout");
  manager.addNode(n);

  DEBUG("%s",object2string(to_object(n)).c_str());
  manager.eval("print (dir(timeout))");
  manager.eval("print ('Current timeout is ' .. timeout.timeout)");

  FAIL_IF_NOT( check(manager, "true") );
  FAIL_IF( check(manager, "timeout == nil") );
  FAIL_IF_NOT( check(manager, "timeout.timeout == 9") );
  FAIL_IF_NOT( check(manager, "timeout.timeout > 8") );
  FAIL_IF( check(manager, "timeout.timeout > 9") );
  FAIL_IF( check(manager, "timeout.timeout > 10") );
  FAIL_IF_NOT( check(manager, "timeout.timeout < 11") );

  FAIL_IF( check(manager, "tout == nil") );
  FAIL_IF_NOT( check(manager, "tout.timeout == 9") );
  FAIL_IF_NOT( check(manager, "tout.timeout > 8") );
  FAIL_IF( check(manager, "tout.timeout > 9") );
  FAIL_IF( check(manager, "tout.timeout > 10") );
  FAIL_IF_NOT( check(manager, "tout.timeout < 11") );

  END_LOCAL();
}

void t03_fullcode()
{
  INIT_LOCAL();

  Manager manager;
  Node::p n=std::make_shared<Timeout>();
  n->setName("tout");
  n->setAttr("timeout","9.0");
  manager.addNode(n);

  FAIL_IF_EXCEPTION(
    manager.eval(
      "for i=1,10 do\n"
      "  print(i);\n"
      "end\n"
    )
  );
  FAIL_IF_NOT_EXCEPTION(
    manager.eval(
      "for i=1,10 do\n"
      "  print(i..' '..getvar('timeout.count'))\n"
      "end\n"
    )
  );
  FAIL_IF_EXCEPTION(
    manager.eval(
      "for i=1,10 do\n"
      "  print(i..' '..timeout.count)\n"
      "end\n"
    )
  );

  END_LOCAL();
}

std::string check_print_what;
bool check_print_ok=false;

void check_print(const std::string &str)
{
  printf("LUA -- %s\n",str.c_str());

  check_print_ok=(std::search(str.begin(),str.end(), check_print_what.begin(),check_print_what.end())!=str.end());
}

void t04_set_node_value_at_lua()
{
  INIT_LOCAL();

  Manager manager;

  Node::p n=std::make_shared<Timeout>();
  FAIL_IF_EXCEPTION( n->setAttr("timeout","9") );
  n->setName("tout");
  manager.addNode(n);

  FAIL_IF_NOT( check(manager, "tout.timeout == 9") );
  FAIL_IF_EXCEPTION( manager.eval("tout.timeout = 10") );
  FAIL_IF( check(manager, "tout.timeout == 9") );
  FAIL_IF_NOT( check(manager, "tout.timeout == 10") );

  manager.eval("print(tout._ptr)");

  FAIL_IF_EXCEPTION(
    manager.eval(
      "print('tout {')\n"
      "for k,v in pairs(tout.param_list) do\n"
      "  print('  '.. k .. ' = ' .. v)\n"
      "end\n"
      "print('}')\n"
    )
  );

  lua_ab_print_real=check_print;

  check_print_what="tout";
  check_print_ok=false;
  manager.eval("print(dir())");
  FAIL_IF_NOT(check_print_ok);

  check_print_what="timeout";
  check_print_ok=false;
  manager.eval("print(tout)");
  FAIL_IF_NOT(check_print_ok);

  check_print_what="timeout";
  check_print_ok=false;
  manager.eval("print(dir(tout))");
  FAIL_IF_NOT(check_print_ok);

  END_LOCAL();
}

void t05_parse_string()
{
  INIT_LOCAL();

  Manager manager;

  Node::p n=std::make_shared<Timeout>();
  FAIL_IF_EXCEPTION( n->setAttr("timeout","9") );
  n->setName("tout");
  manager.addNode(n);

  FAIL_IF_NOT_EQUAL_STRING(manager.parseString("Nr {{tout.timeout}}"),"Nr 9");

  FAIL_IF_NOT_EQUAL_STRING(manager.parseString("Nr {{tout.timeout}}."),"Nr 9.");

  manager.eval("my_var='Hello world'");

  FAIL_IF_NOT_EQUAL_STRING(manager.parseString("He sais: {{my_var}}."),"He sais: Hello world.");
  FAIL_IF_NOT_EQUAL_STRING(manager.parseString("He sais: {{my_var}}, and timeout is {{tout.timeout}}.tail."),"He sais: Hello world, and timeout is 9.tail.");

  END_LOCAL();
}

int main(int argc, char **argv)
{
  START();

  t00_resolve();
  t01_simple();
  t02_with_manager();
  t03_fullcode();
  t04_set_node_value_at_lua();
  t05_parse_string();

  END();
  return 0;
}
