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

#ifndef __AB_OBJECT_BASIC_H__
#define __AB_OBJECT_BASIC_H__

#include "object.h"

namespace AB {
  class String : public ObjectBase {
  public:
    static const char *type;

    String(const std::string &_s) : ObjectBase(String::type), s(_s) {}
    std::string s;
  };

  class Integer : public ObjectBase {
  public:
    static const char *type;

    Integer(int _n) : ObjectBase(Integer::type), n(_n) {}
    int n;
  };

  class Float : public ObjectBase {
  public:
    static const char *type;

    Float(double _n) : ObjectBase(Float::type), n(_n) {}
    double n;
  };

  class CallableObject : public ObjectBase {
  public:
    static const char *type;

    CallableObject(boost::function<Object(ObjectList&)> _f) : ObjectBase(CallableObject::type), f(_f) {}

    virtual Object call(ObjectList& params) {
      return f(params);
    }
    boost::function<Object(ObjectList&)> f;
  };
}

#endif
