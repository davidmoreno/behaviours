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

#ifndef AB_ACTION_H
#define AB_ACTION_H

#include <string>

#include "node.h"
#include "log.h"

namespace AB {
  /**
   * @short An action that may be performed.
   *
   * This class must be subclassed by users to add actions to perform.
   */
  class Action : public Node {
  public:
    using p = std::shared_ptr<Action>;
    Action(const char *type) : Node(type) {}

    /**
     * @short Executes the action itself.
     *
     * For example if the action is to play some audio file, do the necesary to play it.
     */
    virtual void exec() { WARNING("::exec not implemented at %s",type()); };
  };
}

#endif
