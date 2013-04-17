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

#ifndef AB_EMPTY_H
#define AB_EMPTY_H

#include <ab/action.h>

namespace AB {
  class Empty : public AB::Action {
  public:
    Empty(const char* type="empty") : AB::Action(type) {}
    void exec() {}
    void setAttr(const std::string &k,AB::Object t) {}
  private:
  };
};

#endif
