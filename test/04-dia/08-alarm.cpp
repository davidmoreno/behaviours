#include <aisoy1/log.h>
#include <ab/log.h>
#include <ab/manager.h>
#include <signal.h>
#include "../../dia/events/alarm.h"
#include "../../dia/dia.h"

#undef ERROR
#include "../tests.h"

AB::Manager manager;

#ifdef _TEST_

void t01_general(int argc, char **argv)
{
  INIT_LOCAL();

  DIA::Alarm *alarm = new DIA::Alarm;
  DIA::AlarmManager *alarmM = new DIA::AlarmManager;
  
  alarm->setAttr("minute", AB::to_object(3));
  alarm->setAttr("hour", AB::to_object(15));
  alarm->setAttr("month", AB::to_object(5)); // June, GUI array starts at January 
  alarm->setAttr("day", AB::to_object(24)); // 25: GUI array starts at 0
  alarm->setAttr("year", AB::to_object(0)); // 2012: GUI array starts at 2012 [2012, 2013, ... ]
  alarm->setAttr("repeat", AB::to_object((int)DIA::Alarm::Never));
   
  DIA::init(&manager);

  manager.addNode(alarmM);
  manager.addNode(alarm);

  INFO("Alarm set to: %d:%d, %d/%d/%d  (%d)",alarm->getHour(), alarm->getMinute(), alarm->getDay(), alarm->getMonth()+1, alarm->getYear()+1900, (int)alarm->getRepeatPolicy());
  
  struct tm fakeDate;
  fakeDate.tm_sec = 0; // 0-61
  fakeDate.tm_min = 3; //0-59
  fakeDate.tm_hour = 15; //0-23
  fakeDate.tm_mday = 25; // 1-31, monday
  fakeDate.tm_mon = 5; // 0-11, june
  fakeDate.tm_year = 112; // year starts in 1900
  
  time_t fakeTime = mktime(&fakeDate);
  INFO("Current time: %d:%d:%d, %d/%d/%d (%d) ",fakeDate.tm_hour, fakeDate.tm_min, fakeDate.tm_sec, fakeDate.tm_mday, fakeDate.tm_mon+1, fakeDate.tm_year+1900, fakeDate.tm_wday );  
  // Fail if alarm not notified
  FAIL_IF(!alarmM->check(fakeTime)); 

  fakeDate.tm_mday = 2; // 1-31, monday
  fakeDate.tm_mon = 6; // 0-11, july
  fakeTime = mktime(&fakeDate);
  INFO("Current time: %d:%d:%d, %d/%d/%d (%d) ",fakeDate.tm_hour, fakeDate.tm_min, fakeDate.tm_sec, fakeDate.tm_mday, fakeDate.tm_mon+1, fakeDate.tm_year+1900, fakeDate.tm_wday );  
  // Fail if alarm notified (it is a monday, but different date
  FAIL_IF(alarmM->check(fakeTime));
  
  alarm->setAttr("repeat", AB::to_object((int)DIA::Alarm::Always));
  alarm->setAttr("minute", AB::to_object(4));
  INFO("Alarm set to: %d:%d, %d/%d/%d  (%d)",alarm->getHour(), alarm->getMinute(), alarm->getDay(), alarm->getMonth()+1, alarm->getYear()+1900, (int)alarm->getRepeatPolicy());
  fakeDate.tm_min = 4;
  fakeTime = mktime(&fakeDate);
  INFO("Current time: %d:%d:%d, %d/%d/%d (%d) ",fakeDate.tm_hour, fakeDate.tm_min, fakeDate.tm_sec, fakeDate.tm_mday, fakeDate.tm_mon+1, fakeDate.tm_year+1900, fakeDate.tm_wday );  
  // Fail if alarm not notified
  FAIL_IF(!alarmM->check(fakeTime));
  
  alarm->setAttr("repeat", AB::to_object((int)DIA::Alarm::Wednesdays));
  alarm->setAttr("minute", AB::to_object(5));
  INFO("Alarm set to: %d:%d, %d/%d/%d  (%d)",alarm->getHour(), alarm->getMinute(), alarm->getDay(), alarm->getMonth()+1, alarm->getYear()+1900, (int)alarm->getRepeatPolicy());
  fakeDate.tm_min = 5;
  fakeTime = mktime(&fakeDate);
  INFO("Current time: %d:%d:%d, %d/%d/%d (%d) ",fakeDate.tm_hour, fakeDate.tm_min, fakeDate.tm_sec, fakeDate.tm_mday, fakeDate.tm_mon+1, fakeDate.tm_year+1900, fakeDate.tm_wday ); 
  // Fail if alarm notified (it is a monday, not a wednesday)
  FAIL_IF(alarmM->check(fakeTime));
  
  alarm->setAttr("repeat", AB::to_object((int)DIA::Alarm::Mondays));
  alarm->setAttr("minute", AB::to_object(6));
  INFO("Alarm set to: %d:%d, %d/%d/%d  (%d)",alarm->getHour(), alarm->getMinute(), alarm->getDay(), alarm->getMonth()+1, alarm->getYear()+1900, (int)alarm->getRepeatPolicy());
  fakeDate.tm_min = 6;
  fakeTime = mktime(&fakeDate);
  INFO("Current time: %d:%d:%d, %d/%d/%d (%d) ",fakeDate.tm_hour, fakeDate.tm_min, fakeDate.tm_sec, fakeDate.tm_mday, fakeDate.tm_mon+1, fakeDate.tm_year+1900, fakeDate.tm_wday );  
  // Fail if alarm not notified (it is a monday, but different date)
  FAIL_IF(!alarmM->check(fakeTime));
  
  alarm->setAttr("repeat", AB::to_object((int)DIA::Alarm::Weekdays));
  INFO("Alarm set to: %d:%d, %d/%d/%d  (%d)",alarm->getHour(), alarm->getMinute(), alarm->getDay(), alarm->getMonth()+1, alarm->getYear()+1900, (int)alarm->getRepeatPolicy());
  fakeDate.tm_mday = 26; // 1-31, monday
  fakeTime = mktime(&fakeDate);
  INFO("Current time: %d:%d:%d, %d/%d/%d (%d) ",fakeDate.tm_hour, fakeDate.tm_min, fakeDate.tm_sec, fakeDate.tm_mday, fakeDate.tm_mon+1, fakeDate.tm_year+1900, fakeDate.tm_wday ); 
  // Fail if alarm not notified (it is a tuesday, a weekday)
  FAIL_IF(!alarmM->check(fakeTime));

  fakeDate.tm_mday = 29; // sunday
  fakeTime = mktime(&fakeDate);
  INFO("Current time: %d:%d:%d, %d/%d/%d (%d) ",fakeDate.tm_hour, fakeDate.tm_min, fakeDate.tm_sec, fakeDate.tm_mday, fakeDate.tm_mon+1, fakeDate.tm_year+1900, fakeDate.tm_wday ); 
  // Fail if alarm notified (it is a saturday, weekend)
  FAIL_IF(alarmM->check(fakeTime));
  
  alarm->setAttr("repeat", AB::to_object((int)DIA::Alarm::Weekends));
  INFO("Alarm set to: %d:%d, %d/%d/%d  (%d)",alarm->getHour(), alarm->getMinute(), alarm->getDay(), alarm->getMonth()+1, alarm->getYear()+1900, (int)alarm->getRepeatPolicy());
  // Fail if alarm not notified (it is a saturday, weekend)
  FAIL_IF(!alarmM->check(fakeTime)); 
  
  END_LOCAL();
}
#endif

int main(int argc, char **argv)
{
  START()
#ifdef _TEST_
  t01_general(argc,argv);
#endif
  END()
}
