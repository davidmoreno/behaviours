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

#ifndef DIA_STOP_H
#define DIA_STOP_H

#include <string>
#include <cstdlib>
#include <ab/action.h>

namespace AB {
  class Stop : public AB::Action {
  public:
    Stop(const char *type = "stop");
    void setManager(AB::Manager* manager);
    void exec();
  private:
    AB::Manager *manager;
  };
}
#endif
