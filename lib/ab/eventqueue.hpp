/**
 * Behaviours - UML-like graphic programming language
 * Copyright (C) 2013 Miguel Sempere & Coralbits S.L.
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

#ifndef _EVENT_QUEUE_H_
#define _EVENT_QUEUE_H_

#include <iostream>
#include "circularbuffer.hpp"
#include <memory>
#include <mutex>
#include <condition_variable>

typedef struct json_object json_object;

namespace AB{
	class EventQueue{
		public:
			EventQueue();
			void pushEvent(const std::string &, json_object *);
			void pushEvent(const std::string &type, const std::string &key, const std::string &value);
			json_object* getEvents(int from_id);
			std::string getJSONString(int from_id);
			std::string getJSONStringBlock(int from_id);
			std::string test(int start_id);

		private:
			CircularBuffer<std::shared_ptr<json_object>> queue;
			int start_id;
			std::mutex mutex;
			std::condition_variable wait_for_event;
	};
}

#endif
