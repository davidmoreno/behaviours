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

#ifndef AB_CONNECTION
#define AB_CONNECTION

#include <string>
#include <memory>

#include "node.h"

namespace AB {
  class Node;
  class Manager;

  /**
   * @short A connection between two nodes.
   *
   * A connection means that when node A is notified then node B may eb notified too. It depends on
   * the guard that it will notified or not and on other Connections on the same A, as described at
   * AB::Manager::notify.
   */
  class Connection {
    Manager *manager;
    Node::p from_;
    Node::p to_;
    std::string guard_;
    std::string name_;
  public:
		using p = std::shared_ptr<Connection>;
		
    /**
     * @short Creates a connection on the given manager
     */
    Connection(Manager *manager, Node::p from, Node::p to);
    /**
     * @short Sets the guard expression for this connection
     */
    bool setGuard(const std::string &guard);
    /**
     * @short Checks if the guard validates
     */
    bool checkGuard();
    /**
     * @short Returns the to node
     */
    Node::p to() { return to_; };
    /**
     * @short Returns the from node
     */
    Node::p from() { return from_; };
    /**
     * @short Returns the guard itseld.
     */
    std::string guard() { return guard_; }
    /**
     * @short Returns he current name of the connection 
     */
    const std::string &name() const { return name_; }
    /**
     * @short Returns a printable version of the connection, usefull for printing connection graph.
     */
    std::string unicode();
  };
}

#endif
