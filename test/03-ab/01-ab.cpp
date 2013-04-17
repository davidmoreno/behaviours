#include "../tests.h"

#include <ab/manager.h>
#include <ab/action.h>
#include <ab/event.h>

using namespace AB;

class Counter : public Action {
  int n;
public:
  Counter() : Action("Counter") {
    n=0;
  }
  virtual Object attr(const std::string& name) {
    if (name=="n")
      return to_object(n);
    return Action::attr(name);
  }
  virtual void exec() {
    n+=1;
  }
};

void t01_programatic_graph()
{
  INIT_LOCAL();
  Manager manager;

  Event *ev=new Event("ev");
  Action *counter=new Counter();

  manager.addNode(ev);
  manager.addNode(counter);


  manager.connect(ev, counter);

  manager.notify(counter);
  FAIL_IF_NOT_EQUAL_INT(object2int(counter->attr("n")),1);

  manager.notify(ev);
  FAIL_IF_NOT_EQUAL_INT(object2int(counter->attr("n")),2);


  Action *lastaction=counter;
  int i;
  for (i=0; i<100; i++) {
    counter=new Counter();
    manager.addNode(counter);
    manager.connect(lastaction,counter);
    lastaction=counter;
  }

  FAIL_IF_NOT_EQUAL_INT(object2int(counter->attr("n")),0);
  manager.notify(ev);
  FAIL_IF_NOT_EQUAL_INT(object2int(counter->attr("n")),1);


  END_LOCAL();
}

int main()
{
  START();

  t01_programatic_graph();

  END();
}
