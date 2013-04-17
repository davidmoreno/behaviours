/*
 * (C) 2013 Coralbits SL & AISoy Robotics.
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AB_OBJECT_H
#define AB_OBJECT_H

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <exception>
#include <vector>
#include <string>
#include <boost/bind.hpp>

struct lua_State;

namespace AB {
  class object_not_convertible : public std::exception {
    std::string msg;
  public:
    object_not_convertible(const std::string &d) {
      msg=std::string("Object not convertible to ")+d;
    };
    object_not_convertible(const std::string &o,const std::string &d) {
      msg=o+(" not convertible to ")+d;
    };
    virtual ~object_not_convertible() throw() {};
    const char *what() const throw() { return msg.c_str(); }
  };
  class attribute_not_found : public std::exception {
    std::string msg;
  public:
    attribute_not_found(const std::string &s) { msg="Attribute not found: "+s; }
    ~attribute_not_found() throw() {}
    const char *what() const throw() { return msg.c_str(); }
  };
  class attribute_read_only : public std::exception {
    std::string msg;
  public:
    attribute_read_only(const std::string &s) { msg="Read-Only attribute: "+s; }
    ~attribute_read_only() throw() {}
    const char *what() const throw() { return msg.c_str(); }
  };
  class object_not_callable : public std::exception {
  public:
    const char *what() const throw() { return "Object not callable"; }
  };

  class ObjectBase;
  typedef boost::shared_ptr<ObjectBase> Object;
  typedef std::vector<Object> ObjectList;
  typedef std::vector<std::string> AttrList;

  static inline Object to_object(Object o) { return o; } // Idempotent
  Object to_object(const std::string &);
  static inline Object to_object(const char *str) { return to_object(std::string(str)); }
  Object to_object(int);
  Object to_object(double);
  Object to_object(boost::function<Object(ObjectList)> f);

  /**
   * @short Generic object. All data data goes through the scripting languages are of this type.
   *
   * It is designed to be easily convertible to necesary scripting languages, and be ampliable.
   *
   * Each object has attributes, many modeled after python. Objects may be callable.
   * There are some builtin attributes:
   *
   * * __str__ -- When called converts the object to its string representation
   * * __int__ -- When called converts it to an integer
   * * __float__ -- Converts to float
   *
   * * __lua__ -- Pushes the data on the LUA stack, with the given lua_State. @see lua.cpp
   */
  class ObjectBase {
    const char *_type;
  public:

    /// Shortcut to avoid creating it a lot.
    static const AttrList emptyAttrList;

    ObjectBase(const char *type);
    virtual ~ObjectBase();

    virtual Object attr(const std::string &name) { throw(attribute_not_found(name)); }
    virtual void setAttr(const std::string &name, Object obj) { throw(attribute_not_found(name)); }
    virtual AttrList attrList() { return ObjectBase::emptyAttrList; }
    virtual Object call(ObjectList &params) { throw(object_not_callable()); }

    // Some sugar
    Object call() {
      ObjectList ol;
      return call(ol);
    }
    template<typename A>
    Object call(A a) {
      ObjectList ol;
      ol.push_back( to_object(a) );
      return call(ol);
    }
    const char *type() const { return _type; }

    template<typename A>
    Object method2object(Object (A::*f)(ObjectList &)) {
      return to_object(boost::bind(f,static_cast<A*>(this),_1));
    }
    template<typename A>
    Object method2object(Object (A::*f)()) {
      return to_object(boost::bind(f,static_cast<A*>(this)));
    }

    // Easy setAttrs
    template<typename A>
    void setAttr(const std::string &name, const A &v) {
      setAttr(name,to_object(v));
    }

  };


  //Object function2object(boost::function<Object(void)> f);



  /// Converts the object to a string
  std::string object2string(const Object &);
  /// Converts the object to a int, or throws not convertible.
  int object2int(const Object &);
  /// Converts the object to a int, or throws not convertible.
  double object2float(const Object &);
};

#endif

