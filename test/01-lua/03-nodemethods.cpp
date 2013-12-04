#include "../tests.h"

#include <ab/action.h>
#include <ab/manager.h>
#include <ab/luaobject.h>

extern "C" {
#include <lua.h>
}


using namespace AB;


class Size : public LUAObject {
  int w,h;
public:
  Size(int _w, int _h) : w(_w), h(_h) {}

  int to_lua(lua_State *s) {
    DEBUG("Push size %dx%d",w,h);
    lua_newtable(s);
    lua_pushstring(s,"w");
    lua_pushinteger(s,w);
    lua_settable(s,-3);
    lua_pushstring(s,"h");
    lua_pushinteger(s,h);
    lua_settable(s,-3);
    return 1;
  };
};

class Image : public ObjectBase {
  std::shared_ptr<Size> size;
public:
  Object sizeCallable;

  Image(int w, int h) : ObjectBase("Image"), size(new Size(w,h)) {
    sizeCallable=method2object<Image>(&Image::get_size);
  }

  Object get_size(ObjectList &l) {
    return size;
  }

  Object hello(ObjectList &l) {
    return to_object("Hello");
  }

  int size_to_lua(lua_State *s) {
    DEBUG("per attr");
    return size->to_lua(s);
  }

  virtual Object attr(const std::string &attr) {
    if (attr=="size")
      return sizeCallable;
    if (attr=="hello")
      return method2object(&Image::hello);
    if (attr=="size_attr")
      return lua_bridge(&Image::size_to_lua,this); // This is now an attribute, not a function.
    return ObjectBase::attr(attr);
  }
};

class TestNode : public Action {
public:
	using p = std::shared_ptr<TestNode>;
	
  int ntests;
  int lastnargs;

  TestNode() : Action("testnode") {
    ntests=0;
    lastnargs=0;
  };

  Object test(ObjectList &ol) {
    lastnargs=ol.size();
    ntests++;
    INFO("Test called: %d params",lastnargs);
    if (lastnargs>1) {
      int n=object2int(ol[0]);
      return ol[n];
    }
    return to_object(0);
  }

  Object getImage(ObjectList &ol) {
    Object img=std::shared_ptr<ObjectBase>(new Image(320,200));
    return img;
  }

  virtual Object attr(const std::string& name) {
    if (name=="test")
      return method2object<TestNode>(&TestNode::test);
    if (name=="getImage")
      return method2object<TestNode>(&TestNode::getImage);
    return Node::attr(name);
  }


};


void t01_node_with_methods()
{
  INIT_LOCAL();

  Manager manager;
  TestNode::p node=std::make_shared<TestNode>();

  manager.addNode(node);

  manager.eval("print(testnode)");
  manager.eval("print(testnode.test)");
  manager.eval("print(dir(testnode))");
  FAIL_IF_NOT_EQUAL_INT(node->ntests,0);
  DEBUG("***");
  manager.eval("testnode.test()");
  DEBUG("***");
  FAIL_IF_NOT_EQUAL_INT(node->ntests,1);
  FAIL_IF_NOT_EQUAL_INT(node->lastnargs,0);
  manager.eval("testnode.test()");
  FAIL_IF_NOT_EQUAL_INT(node->ntests,2);
  FAIL_IF_NOT_EQUAL_INT(node->lastnargs,0);
  manager.eval("testnode.test(1)");
  FAIL_IF_NOT_EQUAL_INT(node->ntests,3);
  FAIL_IF_NOT_EQUAL_INT(node->lastnargs,1);
  manager.eval("testnode.test(1,2)");
  FAIL_IF_NOT_EQUAL_INT(node->ntests,4);
  FAIL_IF_NOT_EQUAL_INT(node->lastnargs,2);
  AB::Object ob=manager.eval("return testnode.test(1,2,'Hello')");
  FAIL_IF_NOT_EQUAL_INT(node->ntests,5);
  FAIL_IF_NOT_EQUAL_INT(node->lastnargs,3);
  FAIL_IF_NOT_EQUAL_INT(object2int(ob),2);

  ob=manager.eval("return testnode.test(2,2,'Hello')");
  FAIL_IF_NOT_EQUAL_STRING(object2string(ob),"Hello");

  END_LOCAL();
}

void t02_image_objects()
{
  INIT_LOCAL();

  Manager manager;
  TestNode::p node=std::make_shared<TestNode>();

  manager.addNode(node);
  manager.eval("testnode.test(); img=testnode.getImage();");
  //Object ret=manager.eval("print(img); return img.size();");
  FAIL_IF_NOT_EQUAL_STRING(object2string(manager.eval("return (img.hello() .. ' from LUA')")),"Hello from LUA");


  END_LOCAL();
}

void t03_return_lua_object()
{
  INIT_LOCAL();

  Manager manager;
  TestNode::p node=std::make_shared<TestNode>();

  manager.addNode(node);
  manager.eval("testnode.test(); img=testnode.getImage();");
  manager.eval("print(img); print(img.size()); return img.size();");
  Object w=manager.eval("return img.size().w;");
  FAIL_IF_NOT_EQUAL_INT(object2int(w),320);
  Object h=manager.eval("return img.size().h;");
  FAIL_IF_NOT_EQUAL_INT(object2int(h),200);

  w=manager.eval("return img.size_attr.w;");
  FAIL_IF_NOT_EQUAL_INT(object2int(w),320);
  h=manager.eval("return img.size_attr.h;");
  FAIL_IF_NOT_EQUAL_INT(object2int(h),200);

  END_LOCAL();
}

int main(void)
{
  START();

  t01_node_with_methods();
  t02_image_objects();
  t03_return_lua_object();

  END();
}
