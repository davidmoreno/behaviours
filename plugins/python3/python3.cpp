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
#include "python3.hpp"



namespace AB{
	class Python3Action : public Action{
		std::string code;
		PyObject *compiled_code;
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
		PyObject *compiled_code;
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
		PyObject *globals;
		PyObject *PyInit_ab(void);
		void python3_init();
		Manager *ab_module_manager=NULL;
	}
}



void python3_interpreter(FILE *fd){
	sleep(5);
	PyRun_InteractiveLoopFlags(fd, "__stdin__", (PyCompilerFlags*)NULL);
}

void ab_init(void){
	DEBUG("Loaded python3 plugin");
	AB::Factory::registerClass<AB::Python3Action>("python3action");
	AB::Factory::registerClass<AB::Python3Event>("python3event");

	AB::Python3::python3_init();
	
	//new boost::thread(python3_interpreter, stdin);
}

using namespace AB;
using namespace AB::Python3;

void AB::Python3::python3_init(){
	PyImport_AppendInittab("ab",PyInit_ab);
	Py_SetProgramName((wchar_t*)("behaviours"));
	Py_Initialize();

	globals=PyDict_New();
	PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());	
	
	PyEval_InitThreads();
	
	FILE *init_fd=fopen( AB_PREFIX "/share/ab/python3/__init__.py", "ro" );
	if (!init_fd){
		ERROR("Could not execute __init__ to set p a proper python3 environment");
	}
	else{
		PyObject *init_ret=PyRun_File(init_fd, "__init__.py", Py_file_input, globals, globals);
		Py_XDECREF(init_ret);
		
		fclose(init_fd);
	}
}

Python3Action::Python3Action(const char* type): Action(type)
{
	code="";
	compiled_code=NULL;
}

Python3Action::~Python3Action()
{
	Py_XDECREF(compiled_code);
}

void Python3Action::exec()
{
	if (code.size()==0)
		return;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this) ;
	PyDict_SetItemString(locals, "self", object2pyobject( o ));
	
	PyObject *obj=PyRun_String( code.c_str() ,Py_file_input, globals, locals);
	if (!obj)
		PyErr_Print();
	else
		Py_DECREF(obj);
	Py_DECREF(globals);
	Py_DECREF(locals);
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
	if (name=="code"){
		code=object2string(obj);
		Py_XDECREF(compiled_code);
		compiled_code=Py_CompileString(code.c_str(), this->name().c_str(), Py_file_input);
		if (!compiled_code)
			PyErr_Print();
	}
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
	code="";
	setFlags(flags()|AB::Event::Polling);
	compiled_code=NULL;
}

Python3Event::~Python3Event()
{
	Py_XDECREF(compiled_code);
}

bool Python3Event::check()
{
	DEBUG("Check");
	if (!compiled_code)
		return false;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this) ;
	PyDict_SetItemString(locals, "self", object2pyobject( o ));
	
	PyObject *obj=PyEval_EvalCode( compiled_code, globals, locals);
	if (!obj)
		PyErr_Print();
	else{
		PyObject_Print(obj, stdout, Py_PRINT_RAW);
		Py_DECREF(obj);
	}
	Py_DECREF(globals);
	Py_DECREF(locals);
	return false;
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
	if (name=="code"){
		code=object2string(obj);
		Py_XDECREF(compiled_code);
		compiled_code=Py_CompileString(code.c_str(), this->name().c_str(), Py_file_input);
		if (!compiled_code)
			PyErr_Print();
	}
	else
		return Event::setAttr(name, obj);
}

void Python3Event::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Python3::ab_module_manager=manager;
}
