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

#ifndef AB_BLOCKING_ACTION_H
#define AB_BLOCKING_ACTION_H

#include <string>
#include <condition_variable>
#include <thread>
#include "action.h"
#include "log.h"

namespace AB {
  /**
   * @short A BlockingAction that may be performed.
   *
   * This class must be subclassed by users to add actions to perform.
   */
  class BlockingAction : public Action {
  public:
    BlockingAction(const char *type) : Action(type) {}

    /**
     * @short Executes the BlockingAction itself.
     *
     * For example if the BlockingAction is to play some audio file, do the necesary to play it.
     */
    virtual void exec() {
      continuar = false;
      {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, [this]{return continuar;});
      }
    };
    std::condition_variable cv;
    std::mutex cv_m;
    bool continuar;
    std::thread::id idThread;
  private:
    
  };
}

#endif
