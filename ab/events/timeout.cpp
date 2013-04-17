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

#include <ab/log.h>
#include <sstream>
#include "timeout.h"
#include <stdlib.h>
#include <sys/time.h>

using namespace AB;

Timeout::Timeout(const char* type) : Event(type)
{
  setFlags(Polling|NeedSync);
  limit=-1;
  t=-1;
  n=0;
}

Object Timeout::attr(const std::string& k)
{
  if (k=="count")
    return to_object(n);
  else if (k=="timeout")
    return to_object(limit);
  else if (k=="elapsed" || k=="t")
    return to_object(t);
  return Node::attr(k);
}

AttrList Timeout::attrList()
{
  DEBUG("Get param list");
  AttrList l=Node::attrList();
  l.push_back("timeout");
  l.push_back("count");
  l.push_back("elapsed");
  return l;
}

void Timeout::setAttr(const std::string& k, Object v)
{
  DEBUG("set attr %s = %s",k.c_str(), object2string(v).c_str());
  if (k=="timeout") {
    limit=AB::object2float(v);
    DEBUG("Set timeout limit %f",limit);
  } else if (k=="elapsed" ||  k=="t" ) {
  // read_only attribute
  // t=AB::object2float(v);
  // DEBUG("Set timeout elapsed to %f",limit);
  } else if (k=="count" ) {
  // read_only attribute
  // n=AB::object2int(v);
  // DEBUG("Set timeout count to %f",limit);
  } else {
    Node::setAttr(k,v);
  }
    
}

bool Timeout::sync()
{
  DEBUG("Sync!");
  n=0;
  t=-1;
  return true;
}

bool Timeout::check()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  if (t>=0) {
    float dt=(now.tv_sec-lastT.tv_sec) + float(now.tv_usec - lastT.tv_usec)/10000000.0;
    t+=dt;
    if (t>limit) {
      n++;
      INFO("Time limit achieved! (count=%d, %f>%f)", n, t, limit);
      t=-1;
      return true;
    }
  } else
    t=0;
  lastT=now;
  return false;
}
