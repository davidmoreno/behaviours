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

#ifndef AB_EVENT_H
#define AB_EVENT_H

#include <string>

#include "node.h"
#include "log.h"

namespace AB {

  class Manager;

  /**
   * @short An Event is a node that checks for some condition and activates when it fulfills.
   *
   * Events are 'spontaneus' changes on the system that will be notified to whoever is listening.
   *
   * An example of an event is a timeout that after N seconds its activated.
   *
   * Events are connected to actions which will make some change on the system, like changing some
   * color or showing something.
   *
   * Events can be purely spontaneus which means that at some moment they will be notified to the
   * manager, or polling. Polling events have a function check called every little time to check
   * if the desired trigger condition meets.
   *
   * Spontaneus events are highly prefered, but they are not always possible. Spontaneus events may
   * have a processing thread that just AB::Manager::notify(...) the event.
   *
   * On some conditions its necesary to have event group managers that are the ones that really
   * do the processing. For example the touch events look for a specific place of touch but may be
   * several of them, each consuming the touch events. The solution is transparently create a touch
   * manager which do the polling and which triggers the other touch events when the conditions meet.
   * So on this example the touch_manager is a polling event and the other touch events are 'spontaneus'
   * events triggered by the manager. The manager is created, if does not exist, at AB::Node::setManager time.
   */
  class Event : public Node {
  public:
    /**
     * @short Extra flags for AB::Event
     */
    typedef enum {
      NeedSync=0x0100,       ///< This event needs to be "synced" before any call to check, if the graph has changed.
      Polling=0x0200,        ///< This event must be polled to check if it happened.
    } Flag;

    typedef int Flags;

    /**
     * @short Constructor of event. Needs the final type name.
     */
    Event(const char *type) : Node(type) {}

    /**
     * @short Function to be called from time to time, to check if the event happends. Check flags.
     */
    virtual bool check() {  WARNING("::check not implemented at %s",type());  return false; };
    /**
     * @short Function to be called if sync flag exists, to indicate that the graph has changed.
     *
     * User may reimplement it if this event needs to perform some calculations if the graph structure changes,
     * for example new nodes are added or substracted.
     */
    virtual bool sync() { return false; };
  };
}

#endif
