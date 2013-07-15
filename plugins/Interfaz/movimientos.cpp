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
#include "movimientos.hpp"
#include "botones.hpp"



namespace AB{
	class InterfazAction : public Action{
		std::string code;
		PyObject *compiled_code;
	public:
		InterfazAction(const char* type);
    virtual ~InterfazAction();
		virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	
	class InterfazEvent : public Action{
		std::string code;
		PyObject *compiled_code;
	public:
		InterfazEvent(const char* type);
    virtual ~InterfazEvent();
    virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	class Up : public Action{
		std::string code;
		PyObject *compiled_code;
	public:
		Up(const char* type);
    virtual ~Up();
		virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	class Down : public Action{
		std::string code;
		PyObject *compiled_code;
	public:
		Down(const char* type);
    virtual ~Down();
		virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	namespace Interfaz{
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
	AB::Factory::registerClass<AB::InterfazAction>("derecha");
	AB::Factory::registerClass<AB::InterfazEvent>("izquierda");
	AB::Factory::registerClass<AB::Up>("arriba");
	AB::Factory::registerClass<AB::Down>("abajo");
	AB::Factory::registerClass<Botones>("botones");

	AB::Interfaz::python2_init();
	
	//new boost::thread(python2_interpreter, stdin);
}

void ab_finalize(void){
	Py_Finalize();
}


using namespace AB;
using namespace AB::Interfaz;

void AB::Interfaz::python2_init(){
	//TODO PyImport_AppendInittab("ab",PyInit_ab);
	//TODO Py_SetProgramName("behaviours");
	Py_Initialize();
	PyInit_ab();
	
	globals=PyDict_New();
	PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());	
	
	PyEval_InitThreads();
	
	FILE *init_fd=fopen( AB_PREFIX "/shared/ab/Interfaz/__init__.py", "ro" );
	if (!init_fd){
		ERROR("Could not execute __init__ to set p a proper python2 environment");
	}
	else{
		PyObject *init_ret=PyRun_File(init_fd, "__init__.py", Py_file_input, globals, globals);
		Py_XDECREF(init_ret);
		
		fclose(init_fd);
	}
}

InterfazAction::InterfazAction(const char* type): Action(type)
{
	code="";
	compiled_code=NULL;
}

InterfazAction::~InterfazAction()
{
	Py_XDECREF(compiled_code);
}

void InterfazAction::exec()
{
	if (code.size()==0)
		return;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this) ;
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

AttrList InterfazAction::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object InterfazAction::attr(const std::string& name)
{
	return to_object(code);
}

void InterfazAction::setAttr(const std::string& name, Object obj)
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

void InterfazAction::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
/********************************************************UP*/
Up::Up(const char* type): Action(type)
{
	code="";
	compiled_code=NULL;
}

Up::~Up()
{
	Py_XDECREF(compiled_code);
}

void Up::exec()
{
	if (code.size()==0)
		return;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this) ;
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

AttrList Up::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object Up::attr(const std::string& name)
{
	return to_object(code);
}

void Up::setAttr(const std::string& name, Object obj)
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

void Up::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
/************/

/********************************************************Down*/
Down::Down(const char* type): Action(type)
{
	code="";
	compiled_code=NULL;
}

Down::~Down()
{
	Py_XDECREF(compiled_code);
}

void Down::exec()
{
	if (code.size()==0)
		return;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this) ;
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

AttrList Down::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object Down::attr(const std::string& name)
{
	return to_object(code);
}

void Down::setAttr(const std::string& name, Object obj)
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

void Down::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
/************/
InterfazEvent::InterfazEvent(const char* type): Action(type)
{
	code="";

	compiled_code=NULL;
}

InterfazEvent::~InterfazEvent()
{
	Py_XDECREF(compiled_code);
}

void InterfazEvent::exec()
{
	if (code.size()==0)
		return;
	
	PyObject *locals=PyDict_New();
	Py_INCREF(globals);
	
	auto o=to_object(this) ;
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

AttrList InterfazEvent::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	return attr;
}

Object InterfazEvent::attr(const std::string& name)
{
	return to_object(code);
}

void InterfazEvent::setAttr(const std::string& name, Object obj)
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

void InterfazEvent::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
