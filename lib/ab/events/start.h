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

#ifndef __AB_START__
#define __AB_START__

#include <ab/event.h>

namespace AB {
  class Start : public AB::Event {
  public:
    Start(const char* type = "start") : Event(type) { setFlags(Polling|NeedSync); }
    virtual bool sync() {
      setFlags(Polling|NeedSync);
      return false;
    }
    virtual bool check() {
      setFlags(NeedSync);
      return true;
    }
  };
}

#endif
