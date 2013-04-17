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

#ifndef FACTORY_H
#define FACTORY_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <map>
#include "node.h"


namespace AB {
  class Event;
  class Action;
  class Node;


  /**
   * @short Generic factory to create new nodes.
   *
   * Node creators must be registered, and the the creators can be called.
   */
  class Factory {
    static std::map<std::string, boost::function<Node*(const std::string& type)> > knownTypes;

    template<class T>
    static AB::Node *createNode(const std::string& type) {
      return new T(type.c_str());
    }
    
  public:
    /**
     * @short This desired type to create does not exist.
     */
    class type_does_not_exists : public std::exception {
      std::string msg;
    public:
      type_does_not_exists(const std::string &t) { msg="Type does not exist: "+t; };
      ~type_does_not_exists() throw() {};
      const char *what() const throw() { return msg.c_str(); }
    };

    /**
     * @short Registers a new creator for a class object
     */
    static bool registerCreator(const std::string &type, boost::function<Node*(const std::string& type)> f);

    /**
     * @short Template to ease the adding of new types.
     *
     * Normally user will add types like:
     *
      @code
      AB::Factory::registerClass<DIA::Listen>("listen");
      @endcode
     */
    template<class T>
    static bool registerClass(const std::string &type) {
      
      registerCreator(type, createNode<T>);
      return true;
    }
    /**
     * @short Creates a node of the desired type, if registered.
     *
     * @returns The Node or throws
     * @throw type_does_not_exists Could not create that type
     */
    static Node *createNode(const std::string &type);
    /**
     * @short Helper to create an event.
     *
     * It calls the createNode and dynamic casts the result, with care about memory.
     * @throw type_does_not_exists Could not create that type
     */
    static Event *createEvent(const std::string &type);
    /**
     * @short Helper to create an action.
     *
     * It calls the createNode and dynamic casts the result, with care about memory.
     * @throw type_does_not_exists Could not create that type
     */
    static Action *createAction(const std::string &type);

    /**
      * @short Returns a list with all known type names
      * 
      * @return The list of known type names
      */
      static std::vector<std::string> list();
		
    /**
     * @short Debugger helper that just returns all the known elements as a string
     *
     * @returns A string with data about all known types.
     */
    static std::string list_as_string();
  };
}

#endif
