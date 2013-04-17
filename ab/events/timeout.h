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

#ifndef AB_TIMEOUT_H
#define AB_TIMEOUT_H

#include <sys/time.h>
#include <ab/event.h>

namespace AB {
  class Timeout : public AB::Event {
    float limit;
    float t;
    int n;
    struct timeval lastT;
  public:
    Timeout(const char* type = "timeout");
    virtual bool check();
    virtual bool sync();
    virtual void setAttr(const std::string &k, Object v);
    virtual Object attr(const std::string &k);
    virtual AttrList attrList();
  };
};

#endif
