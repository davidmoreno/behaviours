#include <iostream>
#include <sys/stat.h>

#include <ab/manager.h>
#include <ab/action.h>
#include <ab/lua.h>
#include "../tests.h"
#include <dia/luacv/luacvaux.h>

using namespace AB;

void t01_useluacv()
{
  INIT_LOCAL();

  unlink("lena-out.png");

  Manager manager;

  manager.eval("cv=require(\"luacv\")");

  manager.eval("lena=cv.LoadImage('lena.jpg')");
  manager.eval("cv.Rectangle(lena, cv.Point(10,10), cv.Point(100,100), cv.CV_RGB(0,255,0), 2, 8.0)");
  manager.eval("cv.SaveImage('lena-out.png',lena)");

  struct stat st;
  FAIL_IF (stat("lena-out.png", &st)!=0);


  END_LOCAL();
}

class MyNode : public AB::Action {
public:
  MyNode(const std::string &name) : AB::Action("MyNode") { setName(name); }

  virtual Object attr(const std::string& name) {
    if (name=="setImage")
      return method2object(&MyNode::setImage);
    return AB::Action::attr(name);
  }

  Object setImage(ObjectList &p) {
    DEBUG("Called setImage");

    LUAData data=LUAData::fromObject(p[0]);

    IplImage *img=luacv_checkObject<IplImage>(data.state,data.index,"IplImage");
    FAIL_IF(img==NULL);

    FAIL_IF_NOT_EQUAL_INT(iplWidth(img),252);
    FAIL_IF_NOT_EQUAL_INT(iplHeight(img),203);

    return to_object(0);
  }
};


void t02_luacv_to_ccv()
{
  INIT_LOCAL();

  Manager manager;

  manager.addNode(new MyNode("node"));

  manager.eval("cv=require(\"luacv\")");
  manager.eval("lena=cv.LoadImage('lena.jpg')");
  manager.eval("node.setImage(lena)");

  END_LOCAL();
}

int main()
{
  START();

  t01_useluacv();
  t02_luacv_to_ccv();

  END();
  return 0;
}
