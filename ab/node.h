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

#ifndef AB_NODE_H
#define AB_NODE_H

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "object.h"

#include <vector>
#include <string>
#include <map>

namespace AB {

  class Manager;
  
  /**
    * @short Stores node virtual positions.
    *
    */
  class Point2D {
  public:
   double x;
   double y;
  };
  
  /**
   * @short Common ancestor between actions and events.
   *
   * Actions and events are diferent, but both have some things in common, as a name, some flags, parameters,
   * and connection properties.
   *
   * Then they specialize in checking for something, or executing an action.
   *
   * Even some flags are common, as ManualRemoval.
   */
  class Node : public ObjectBase {
  public:
    enum {
      ManualRemoval=1, ///< Keep this node even after Manager::clear(). Can only be removed manually.
    } Flags; /// Common flags are 8 bits, from 8 to 16 are action or event.

    /**
     * @short To create a new node its important to pass the type name
     *
     * At creation of the node its important to pass the type. It is used on later resolution for the
     * variables and guards.
     */
    Node(const char *type);
    virtual ~Node();

    /**
     * @short Returns he current name of the node 
     */
    const std::string &name() const { return name_; }
    /**
     * @short Returns the flags as set at setFlags()
     */
    int flags() const { return flags_; }
    /**
     * @short Set the flags on this node.
     *
     * Flags may depend on the type of the node, each next subclassing 8 bits:
     *
     *  0-7 : Node
     *  8-15: Action/Event
     * 16-31: Custom flags
     */
    void setFlags(int f) { flags_=f; }

    /**
     * @short Sets the name of the object.
     *
     * This is used for the bindings and for expressions.
     */
    void setName(const std::string &name) { name_=name; }
    void setName();
    /**
    * @short Sets the virtual position of the node.
    *
    * Used when nodes are drawed in a GUI.
    */
    void setPosition(double x = 0.0, double y = 0.0) { position_.x = x; position_.y = y;}
    void setPosition(Point2D p_) { position_ = p_;}
    
    /**
    * @short Node virtual position in X axis.
    *
    * Used when nodes are drawed in a GUI.
    */
    Point2D position() { return position_;}
    
    
    
    /**
     * @short Sets the AB::Manager.
     *
     * Many events and actions need information about the manager, to add auxiliary nodes,
     * or change the status of the manager.
     *
     * User may reimplement it when needed.
     */
    virtual void setManager(Manager *manager) {};

    virtual Object attr(const std::string& name);
    virtual AttrList attrList();
    virtual void setAttr(const std::string& name, Object obj);

    // Easy setAttrs
    template<typename A>
    void setAttr(const std::string &name, const A &v) {
      setAttr(name,to_object(v));
    }

  private:
    std::string name_;
    std::string type_alias;
    int flags_;
    Point2D position_;
  };

  Object to_object(Node *);
  Node *object2node(Object o);
}

#endif
