#include <sstream>
#include <math.h>

#include <ab/log.h>
#include <ab/object.h>

#include "../tests.h"

void t01_basic_object()
{
  INIT_LOCAL();

  AB::Object n=AB::to_object("Hola");
  FAIL_IF_NOT_EQUAL_STR(n->type(),"String");
  FAIL_IF_NOT_EQUAL_STRING(AB::object2string(n),"Hola");

  n=AB::to_object(123);
  FAIL_IF_NOT_EQUAL_STR(n->type(),"Integer");
  FAIL_IF_NOT_EQUAL_STRING(AB::object2string(n),"123");
  FAIL_IF_NOT_EQUAL_INT(AB::object2int(n),123);

  n=AB::to_object(321.23);
  FAIL_IF_NOT_EQUAL_STR(n->type(),"Float");
  FAIL_IF_NOT_EQUAL_STRING(AB::object2string(n),"321.23");
  FAIL_IF_NOT_EQUAL_INT(AB::object2int(n),321);
  FAIL_IF_NOT_EQUAL(AB::object2float(n),321.23);

  END_LOCAL();
}


class Complex : public AB::ObjectBase {
public:
  static const char *type;
  AB::Object real;
  AB::Object imaginary;

  static AB::Object create(double a, double b) { return std::make_shared<Complex>(a,b); }
  Complex(double a, double b) : ObjectBase(Complex::type), real(AB::to_object(a)), imaginary(AB::to_object(b)) {}

  virtual AB::AttrList attrList() {
    AB::AttrList attrs;
    attrs.push_back("__str__");
    return attrs;
  }

  AB::Object attr(const std::string &name) {
    if (name=="__str__")
      return AB::to_object(std::bind(&Complex::str, this, std::placeholders::_1));
    if (name=="__float__")
      return method2object(&Complex::dble);
    if (name=="real")
      return real;
    if (name=="imaginary")
      return imaginary;
    if (name=="len")
      return AB::to_object(std::bind(&Complex::len, this));
    if (name=="len2")
      return method2object(&Complex::len);
    throw(AB::attribute_not_found(name));
  }

  virtual void setAttr(const std::string& name, AB::Object obj) {
    if (name=="real")
      real=obj;
    else if (name=="imaginary")
      imaginary=obj;
    else
      throw(AB::attribute_read_only(name));
  }

  AB::Object str(AB::ObjectList &ol) {
    std::stringstream ss;
    ss<<object2string(real)<<"+"<<object2string(imaginary)<<"i";
    return AB::to_object(ss.str());
  }

  AB::Object dble(AB::ObjectList &ol) {
    return len();
  }

  AB::Object len() {
    double a=AB::object2float(real);
    double b=AB::object2float(imaginary);
    return AB::to_object(sqrt(a*a+b*b));
  }

};
const char *Complex::type="Complex";

void t02_complex_object()
{
  INIT_LOCAL();

  AB::Object cm=Complex::create(1,2);
  FAIL_IF_NOT_EQUAL_STRING(object2string(cm),"1+2i");
  FAIL_IF_NOT_EQUAL(object2float(cm),sqrt(1*1+2*2));
  FAIL_IF_NOT_EQUAL(object2float(cm->attr("real")),1);
  FAIL_IF_NOT_EQUAL(object2float(cm->attr("imaginary")),2);
  FAIL_IF_NOT_EQUAL(object2float(cm->attr("len")->call()),sqrt(1*1+2*2));
  FAIL_IF_NOT_EQUAL(object2float(cm->attr("len2")->call()),sqrt(1*1+2*2));

  cm->setAttr("real",AB::to_object(2));
  FAIL_IF_NOT_EQUAL_STRING(object2string(cm),"2+2i");
  FAIL_IF_NOT_EQUAL(object2float(cm),sqrt(2*2+2*2));

  FAIL_IF_NOT_EXCEPTION(
    cm->setAttr("__len__",AB::to_object(2.1));
  );

  END_LOCAL();
}

int main(void)
{
  START();

  t01_basic_object();
  t02_complex_object();

  END();
}
