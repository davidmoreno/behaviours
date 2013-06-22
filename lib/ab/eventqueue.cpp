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


#include <sstream>

#include "eventqueue.hpp"
#include "log.h"

using namespace AB;

EventQueue::EventQueue() : start_id(0){
}

void EventQueue::pushEvent(const std::string &type, json_object *obj){
	std::lock_guard<std::mutex> lock(mutex);
	json_object *jobj = json_object_new_object();
	json_object *jstring = json_object_new_string(type.c_str());

	json_object_object_add(jobj,"type", jstring);
	json_object_object_add(jobj,"obj", obj);

	std::shared_ptr<json_object> ptr(jobj, json_object_put);

	if (queue.is_full())
		start_id++;
	
	queue.add(ptr);
}

json_object* EventQueue::getEvents(int id){
	std::lock_guard<std::mutex> lock(mutex);
	int skip=0;
	if (id>start_id)
		skip=id-start_id;
	
	std::vector<std::shared_ptr<json_object>> events=queue.read(skip);

	DEBUG("Skip %d elements on the circular queue (first_id on queue is %d). Want from id %d. Got %ld elements.", skip, start_id,id, events.size());

	json_object *jobj = json_object_new_object();
	json_object_object_add(jobj, "id", json_object_new_int(start_id+queue.count()));

	if (events.size()>0){
		json_object *jarray = json_object_new_array();
		
		for (auto &i: events){
			auto obj=json_object_get(i.get());
			json_object_array_add(jarray,obj);
		}

		json_object_object_add(jobj, "data" , jarray);
	}
	return jobj;
}

std::string EventQueue::getJSONString(int from_id){
	json_object *json=getEvents(from_id);
	std::string ret=json_object_get_string(json);
	json_object_put(json);
	return ret;
}


std::string EventQueue::test(int from_id){
	for(int i=0;i<1000;i++){
		json_object *jobj = json_object_new_object();
		json_object *jstring = json_object_new_string("HOLA");
		json_object_object_add(jobj,"MENSAJE", jstring);
		pushEvent("MSG", jobj);
	}
	return getJSONString(from_id);
}
