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

#include <sstream>
#include <string>
#include <stdio.h>

#include "node.h"
#include "log.h"

using namespace AB;

/**
 * @short Creates a random name, so that new obejcts have a valid name always.
 */
static std::string randomName(const std::string &base)
{
  static int n=0;
  n++;
  char tmp[17];
  snprintf(tmp, sizeof(tmp), "%016d", n);
  return base + tmp;
}

/**
 * Default constructor sets a random name.
 */
Node::Node(const char *type) : ObjectBase(type), flags_(0)
{
  name_=randomName("node_");
}

void Node::setName()
{
  name_=randomName("node_");
}

Node::~Node()
{
  //DEBUG("~Node %s", name().c_str());
}


static Object name_of_node(Node *n, ObjectList)
{
  std::stringstream ss;
  ss<<"<"<<n->type()<<" "<<n<<": "<<n->name()<<">";
  return to_object(ss.str());
}

Object Node::attr(const std::string& name)
{
  if (name=="__str__")
    return to_object(std::bind(&name_of_node,this,std::placeholders::_1));
  return AB::ObjectBase::attr(name);
}

AttrList Node::attrList()
{
  return AB::ObjectBase::attrList();
}

void Node::setAttr(const std::string& name, Object obj)
{
  return AB::ObjectBase::setAttr(name,obj);
}


namespace AB {
  Object to_object(Node::p n)
  {
    return std::dynamic_pointer_cast<ObjectBase>(n);
  }
  Node::p object2node(Object o)
  {
    Node::p n=std::dynamic_pointer_cast<Node>(o);
    if (!n)
      throw(object_not_convertible("Node"));
    return n;
  }
}
