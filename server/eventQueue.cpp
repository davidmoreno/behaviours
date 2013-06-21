#include "eventQueue.h"
#include <json/json_object_private.h>
#include <boost/lexical_cast.hpp>



std::string toString(int number){
   return boost::lexical_cast<std::string>( number );
}


EventQueue::EventQueue(){
}

void EventQueue::pushEvent(const std::string &type, json_object obj){
	json_object *jobj = json_object_new_object();
	json_object *jstring = json_object_new_string(type.c_str());

	json_object_object_add(jobj,"type", jstring);
	json_object_object_add(jobj,"obj", &obj);

	std::shared_ptr<json_object> ptr(jobj);

	queue.add(ptr);
}

json_object* EventQueue::getEvents(int id){
	std::vector<std::shared_ptr<json_object>> events;

	queue.read(id,events);
	json_object *jobj = json_object_new_object();
	json_object *jarray = json_object_new_array();
	std::string temp;

	if(events.size() > 0){
		for (std::vector<std::shared_ptr<json_object>>::iterator it = events.begin() ; it != events.end(); ++it)
			json_object_array_add(jarray,(*it).get());
		temp = "messages "+toString(queue.getLastPosition());
	}
	else
		temp = "Empty";

	json_object_object_add(jobj,temp.c_str() , jarray);
	return jobj;
}


std::string EventQueue::test(){
	json_object *jobj = json_object_new_object();
	json_object *jstring = json_object_new_string("HOLA");

	json_object_object_add(jobj,"MENSAJE", jstring);

	pushEvent("MSG", *jobj);
	return json_object_to_json_string(getEvents(1));

}