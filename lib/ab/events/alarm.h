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

#ifndef AB_ALARM_H
#define AB_ALARM_H

#include <ab/event.h>
#include <time.h>

//#define _TEST_

namespace AB {
  class Alarm : public AB::Event {
  public:
    enum Repeat { Never = 0, Always = 1, Weekdays = 2, Weekends = 3, Mondays = 4, Tuesdays = 5, Wednesdays = 6, Thursdays = 7, Fridays = 8, Saturdays = 9, Sundays = 10 };
  public:
		typedef std::shared_ptr<Alarm> p;
		
    Alarm(const char *type = "alarm");

    virtual AB::Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, AB::Object obj);
    virtual AB::AttrList attrList();

    void setManager(AB::Manager* manager);



    int getDay() { return day; };
    int getMonth() { return month; };
    int getYear() { return year; };
    int getHour() { return hour; };
    int getMinute() { return minute; };
    Repeat getRepeatPolicy() { return repeatPolicy; };
    int getNodeon(){return nodeon;}
    int getNoderepeat(){return noderepeat;}

  private:
    int day;
    int month;
    int year;
    int hour;
    int minute;
    Repeat repeatPolicy;
    AB::Manager *manageralarm;
    AB::Event::p event;
  };

  class AlarmManager : public AB::Event {
  public:
    AlarmManager();
    bool check();
#ifdef _TEST_
    bool check(time_t rawtime) {return this->checkAlarm(rawtime);};
#endif

    void setManager(AB::Manager* manager);
    virtual AB::Object attr(const std::string& name);
    virtual AB::AttrList attrList();
  private:
    bool checkAlarm(time_t rawtime);
    time_t lastAlarm;
    bool triggered;
    int noderepeat;
    AB::Manager *manager;
  };
};

#endif
