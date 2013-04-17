#include <ab/log.h>
#include "../tests.h"

#include <ab/manager.h>
#include <ab/node.h>
#include <ab/actions/luaaction.h>
#include <ab/events/luaevent.h>
#include <ab/events/timeout.h>

using namespace AB;

void t01_action_node()
{
  INIT_LOCAL();

  AB::Manager manager;
  AB::Action *n=new AB::LUAAction;
  n->setManager(&manager);
  n->setAttr("exec","print('Hola mundo')");

  n->exec();

  delete n;

  END_LOCAL();
}

void t02_event_node()
{
  INIT_LOCAL();

  AB::Manager manager;
  AB::Event *n=new AB::LUAEvent;
  manager.addNode(n);
  n->setManager(&manager);
  n->setAttr("check","print(LUAEvent.check); return true;");

  FAIL_IF_NOT(n->check());

  n->setAttr("check","return false;");
  FAIL_IF(n->check());

  END_LOCAL();
}

void t03_connected()
{
  INIT_LOCAL();

  AB::Manager manager;
  AB::Event *e=new AB::LUAEvent;
  e->setAttr("sync","print('sync at LUA. Adding runs global.'); runs=0;");
  e->setAttr("check"," runs=runs+1; print(runs .. ' > 3',runs>3); return runs>3;");
  AB::Action *a=new AB::LUAAction;
  a->setAttr("exec","print('ready!'); manager.cancel();");

  manager.addNode(e);
  manager.addNode(a);
  manager.connect(e,a);

  manager.exec();

  END_LOCAL();
}

void t04_notify()
{
  INIT_LOCAL();

  AB::Manager manager;
  AB::Event *e=new AB::LUAEvent;
  e->setAttr("check","manager.notify('cancel');");
  AB::Action *a=new AB::LUAAction;
  a->setName("cancel");
  a->setAttr("exec","print('ready!'); manager.cancel();");

  manager.addNode(e);
  manager.addNode(a);

  manager.exec();

  END_LOCAL();
}

void t05_notify_by_ref()
{
  INIT_LOCAL();

  AB::Manager manager;
  AB::Event *e=new AB::LUAEvent;
  e->setAttr("check","manager.notify(cancel);");
  AB::Action *a=new AB::LUAAction;
  a->setName("cancel");
  a->setAttr("exec","print('ready!'); manager.cancel();");

  manager.addNode(e);
  manager.addNode(a);

  manager.exec();

  END_LOCAL();
}

void t06_status()
{
  INIT_LOCAL();

  Manager manager;
  Node *n=new Timeout();
  n->setName("tout");
  n->setAttr("timeout","9.0");
  manager.addNode(n);

  Node *l=new LUAAction;
  manager.addNode(l);
  l->setAttr("exec","Test\nTest");

  manager.eval("print('one')"); SUCCESS();
  manager.eval("print('two')"); SUCCESS();
  manager.eval("print(3)"); SUCCESS();
  manager.eval("print({'one','two',3})"); SUCCESS();
  manager.eval("local a={}; a['one']=1; print(a)"); SUCCESS();
  manager.eval("print(tout)"); SUCCESS();

  manager.eval("print(dir(tout))"); SUCCESS();
  manager.eval("print(dir())"); SUCCESS();

  FAIL_IF_NOT_EQUAL_STRING(object2string(manager.eval("return tostring('')")), "")
  FAIL_IF_EQUAL_STRING(object2string(manager.eval("return tostring(dir())")), object2string(manager.eval("return tostring(dir(tout))")))

  END_LOCAL();
}

int main(int argc, char **argv)
{
  START();
//   t01_action_node();
//   t02_event_node();
//   t03_connected();
//   t04_notify();
//   t05_notify_by_ref();
  t06_status();

  END();
}
