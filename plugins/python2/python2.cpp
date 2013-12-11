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
#include "python2.hpp"



namespace AB{
	class Python2Action : public Action{
		std::string code;
		PyObject *compiled_code;
	public:
		Python2Action(const char* type="python2action");
    virtual ~Python2Action();
		virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	
	class Python2Event : public Event{
		std::string code;
		PyObject *compiled_code;
	public:
		Python2Event(const char* type="python2event");
    virtual ~Python2Event();
    virtual bool check();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	namespace Python2{
		PyObject *globals;
		PyObject *PyInit_ab(void);
		void python2_init();
		Manager *ab_module_manager=NULL;
	}
}



void python2_interpreter(FILE *fd){
	sleep(5);
	PyRun_InteractiveLoopFlags(fd, "__stdin__", (PyCompilerFlags*)NULL);
}

void ab_init(void){
	DEBUG("Loaded python2 plugin");
	AB::Factory::registerClass<AB::Python2Action>("python2action");
	AB::Factory::registerClass<AB::Python2Event>("python2event");

	AB::Python2::python2_init();
	
	//new boost::thread(python2_interpreter, stdin);
}

void ab_finalize(void){
	Py_Finalize();
}


using namespace AB;
using namespace AB::Python2;

void AB::Python2::python2_init(){
	//TODO PyImport_AppendInittab("ab",PyInit_ab);
	//TODO Py_SetProgramName("behaviours");
	Py_Initialize();
	PyInit_ab();
	
	globals=PyDict_New();
	PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());	
	
	PyEval_InitThreads();
	
	FILE *init_fd=fopen( AB_PREFIX "/share/ab/python2/__init__.py", "ro" );
	if (!init_fd){
		ERROR("Could not execute __init__ to set p a proper python2 environment");
	}
	else{
		PyObject *init_ret=PyRun_File(init_fd, "__init__.py", Py_file_input, globals, globals);
		Py_XDECREF(init_ret);
		
		fclose(init_fd);
	}
}

Python2Action::Python2Action(const char* type): Action(type)
{
	code="";
	compiled_code=NULL;
}

Python2Action::~Python2Action()
{
	Py_XDECREF(compiled_code);
}

void Python2Action::exec()
{
	if (code.size()==0)
		return;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this->shared_from_this()) ;
	PyDict_SetItemString(locals, "self", object2pyobject( o ));
	
	PyObject *obj=PyEval_EvalCode( (PyCodeObject*)compiled_code, globals, locals);
	if (!obj)
		PyErr_Print();
	else{
		PyObject_Print(obj, stdout, Py_PRINT_RAW);
		Py_DECREF(obj);
	}
	Py_DECREF(globals);
	Py_DECREF(locals);
}

AttrList Python2Action::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object Python2Action::attr(const std::string& name)
{
	return to_object(code);
}

void Python2Action::setAttr(const std::string& name, Object obj)
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

void Python2Action::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Python2::ab_module_manager=manager;
}

Python2Event::Python2Event(const char* type): Event(type)
{
	code="";
	setFlags(flags()|AB::Event::Polling);
	compiled_code=NULL;
}

Python2Event::~Python2Event()
{
	Py_XDECREF(compiled_code);
}

bool Python2Event::check()
{
	DEBUG("Check");
	if (!compiled_code)
		return false;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this->shared_from_this());
	PyDict_SetItemString(locals, "self", object2pyobject( o ));
	
	PyObject *obj=PyEval_EvalCode( (PyCodeObject*)compiled_code, globals, locals);
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

AttrList Python2Event::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object Python2Event::attr(const std::string& name)
{
	return to_object(code);
}

void Python2Event::setAttr(const std::string& name, Object obj)
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

void Python2Event::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Python2::ab_module_manager=manager;
}
