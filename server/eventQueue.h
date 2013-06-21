#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include <iostream>
#include <json/json.h>
#include "circularBuffer.hpp"
#include <memory>

class EventQueue{
	public:
		EventQueue();
		void pushEvent(const std::string &, json_object);
		json_object* getEvents(int);
		std::string test();

	private:
		CircularBuffer<std::shared_ptr<json_object>> queue;

};

#endif