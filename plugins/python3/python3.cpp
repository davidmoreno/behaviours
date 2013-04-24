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

#include <Python.h>
#include <ab/action.h>
#include <ab/event.h>
#include <ab/factory.h>
#include <ab/manager.h>

namespace AB{
	class Python3Action : public Action{
		std::string code;
	public:
		Python3Action(const char* type);
    virtual ~Python3Action();
		virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	
	class Python3Event : public Event{
		std::string code;
	public:
		Python3Event(const char* type);
    virtual ~Python3Event();
    virtual bool check();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	namespace Python3{
		PyObject *PyInit_ab(void);
		void python3_init();
		Manager *ab_module_manager=NULL;
	}
}



void ab_init(void){
	DEBUG("Loaded python3 plugin");
	AB::Factory::registerClass<AB::Python3Action>("python3action");
	AB::Factory::registerClass<AB::Python3Event>("python3event");
}

using namespace AB;
using namespace AB::Python3;

static int python_ref_count=0;
void AB::Python3::python3_init(){
	if (python_ref_count==0){
		PyImport_AppendInittab("ab",PyInit_ab);
		Py_SetProgramName((wchar_t*)("behaviours"));
		Py_Initialize();
		
	}
	python_ref_count++;
}

Python3Action::Python3Action(const char* type): Action(type)
{
	python3_init();
	code="";
}

Python3Action::~Python3Action()
{
	python_ref_count--;
	if (python_ref_count==0)
		Py_Finalize();
}

void Python3Action::exec()
{
	if ( PyRun_SimpleString( code.c_str() ) < 0)
		PyErr_Print();
}

AttrList Python3Action::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object Python3Action::attr(const std::string& name)
{
	return to_object(code);
}

void Python3Action::setAttr(const std::string& name, Object obj)
{
	if (name=="code")
		code=object2string(obj);
	else
		return Action::setAttr(name, obj);
}

void Python3Action::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Python3::ab_module_manager=manager;
}

Python3Event::Python3Event(const char* type): Event(type)
{
	python3_init();
	code="";
	setFlags(flags()|AB::Event::Polling);
}

Python3Event::~Python3Event()
{
	python_ref_count--;
	if (python_ref_count==0)
		Py_Finalize();
}

bool Python3Event::check()
{
	if (code.size()==0)
		return false;
	if ( PyRun_SimpleString( code.c_str() ) < 0){
		//PyErr_Print();
		return false;
	}
	else
		return true;
}

AttrList Python3Event::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object Python3Event::attr(const std::string& name)
{
	return to_object(code);
}

void Python3Event::setAttr(const std::string& name, Object obj)
{
	if (name=="code")
		code=object2string(obj);
	else
		return Event::setAttr(name, obj);
}

void Python3Event::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Python3::ab_module_manager=manager;
}
