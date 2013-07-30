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
	class Derecha : public Action{
		std::string code;
		std::string nombre;
		std::string code_default;
		PyObject *compiled_code;
	public:
		Derecha(const char* type);
    virtual ~Derecha();
		virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	
	class Izquierda : public Action{
		std::string code;
		std::string nombre;
		std::string code_default;
		PyObject *compiled_code;
	public:
		Izquierda(const char* type);
    virtual ~Izquierda();
    virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	class Arriba : public Action{
		std::string code;
		std::string nombre;
		std::string code_default;
		PyObject *compiled_code;
	public:
		Arriba(const char* type);
    virtual ~Arriba();
		virtual void exec();
		
    virtual AttrList attrList();
    virtual Object attr(const std::string& name);
    virtual void setAttr(const std::string& name, Object obj);
		
    virtual void setManager(Manager* manager);
	};
	class Abajo : public Action{
		std::string code;
		std::string nombre;
		std::string code_default;
		PyObject *compiled_code;
	public:
		Abajo(const char* type);
    virtual ~Abajo();
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
	AB::Factory::registerClass<AB::Derecha>("derecha");
	AB::Factory::registerClass<AB::Izquierda>("izquierda");
	AB::Factory::registerClass<AB::Arriba>("arriba");
	AB::Factory::registerClass<AB::Abajo>("abajo");
	AB::Factory::registerClass<AB::Botones>("botones");

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

Derecha::Derecha(const char* type): Action(type)
{
	code="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_W(self):\n        self.command='w'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_W()\ntime.sleep(1)\na.stopSerialSend()\na.close()";
	compiled_code=NULL;
	code_default="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_W(self):\n        self.command='w'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_W()\ntime.sleep(1)\na.stopSerialSend()\na.close()";
	nombre = "Derecha";
}

Derecha::~Derecha()
{
	Py_XDECREF(compiled_code);
}

void Derecha::exec()
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

AttrList Derecha::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	attr.push_back("nombre");
	attr.push_back("code_default");
	return attr;
}

Object Derecha::attr(const std::string& name)
{
	if (name == "nombre")
	{
		return to_object(nombre);
	}
	else if(name == "code_default"){
		return to_object(code_default);
	}
	else// if(name=="code"){
		return to_object(code);
	
	
}

void Derecha::setAttr(const std::string& name, Object obj)
{
	if (name=="code"){
		code=object2string(obj);
		Py_XDECREF(compiled_code);
		compiled_code=Py_CompileString(code.c_str(), this->name().c_str(), Py_file_input);
		if (!compiled_code)
			PyErr_Print();
	}
	else if (name == "nombre")
	{
		nombre=object2string(obj);
	}
	else if( name =="code_default"){
		code_default=object2string(obj);
	}
	else
		return Action::setAttr(name, obj);
}

void Derecha::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
/********************************************************Arriba*/
Arriba::Arriba(const char* type): Action(type)
{
	code="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_W(self):\n        self.command='w'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_W()\ntime.sleep(1)\na.stopSerialSend()\na.close()";
	compiled_code=NULL;
	nombre = "Arriba";
	code_default="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_W(self):\n        self.command='w'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_W()\ntime.sleep(1)\na.stopSerialSend()\na.close()";

}

Arriba::~Arriba()
{
	Py_XDECREF(compiled_code);
}

void Arriba::exec()
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

AttrList Arriba::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	attr.push_back("nombre");
	attr.push_back("code_default");
	return attr;
}

Object Arriba::attr(const std::string& name)
{
	if (name == "nombre")
	{
		return to_object(nombre);
	}
	else if(name == "code_default"){
		return to_object(code_default);
	}
	else //if(name=="code"){
		return to_object(code);
	
}

void Arriba::setAttr(const std::string& name, Object obj)
{
	if (name=="code"){
		code=object2string(obj);
		Py_XDECREF(compiled_code);
		compiled_code=Py_CompileString(code.c_str(), this->name().c_str(), Py_file_input);
		if (!compiled_code)
			PyErr_Print();
	}
	else if (name == "nombre")
	{
		nombre=object2string(obj);
	}
	else if( name =="code_default"){
		code_default=object2string(obj);
	}
	else
		return Action::setAttr(name, obj);
}

void Arriba::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
/************/

/********************************************************Abajo*/
Abajo::Abajo(const char* type): Action(type)
{
	code="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_W(self):\n        self.command='w'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_W()\ntime.sleep(1)\na.stopSerialSend()\na.close()";
	compiled_code=NULL;
	nombre = "Abajo";
	code_default="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_W(self):\n        self.command='w'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_W()\ntime.sleep(1)\na.stopSerialSend()\na.close()";
}

Abajo::~Abajo()
{
	Py_XDECREF(compiled_code);
}

void Abajo::exec()
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

AttrList Abajo::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	attr.push_back("nombre");
	attr.push_back("code_default");
	return attr;
}

Object Abajo::attr(const std::string& name)
{
	if (name == "nombre")
	{
		return to_object(nombre);
	}
	else if(name == "code_default"){
		return to_object(code_default);
	}
	else// if(name=="code"){
		return to_object(code);
	
}

void Abajo::setAttr(const std::string& name, Object obj)
{
	if (name=="code"){
		code=object2string(obj);
		Py_XDECREF(compiled_code);
		compiled_code=Py_CompileString(code.c_str(), this->name().c_str(), Py_file_input);
		if (!compiled_code)
			PyErr_Print();
	}
	else if (name == "nombre")
	{
		nombre=object2string(obj);
	}
	else if( name =="code_default"){
		code_default=object2string(obj);
	}
	else
		return Action::setAttr(name, obj);
}

void Abajo::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
/************/
Izquierda::Izquierda(const char* type): Action(type)
{
	code="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_A(self):\n        self.command='a'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_A()\ntime.sleep(1)\na.stopSerialSend()\na.close()";
	nombre = "Izquierda";
	compiled_code=NULL;
	code_default="import time\nclass mov:\n    def __init__(self):\n        import serial\n        self.freedom=serial.Serial('/dev/ttyACM0', 9600)\n        self.freedom.open()\n        self.command='p' #Stop\n\n    def startSerialSend_W(self):\n        self.command='w'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n\n    def stopSerialSend(self):\n        self.command='p'\n        print 'Sending %s' % self.command\n        self.freedom.write(self.command+' \\r\\n')\n        \n    def close(self):\n        self.freedom.close()\n\na = mov()\na.startSerialSend_W()\ntime.sleep(1)\na.stopSerialSend()\na.close()";
}

Izquierda::~Izquierda()
{
	Py_XDECREF(compiled_code);
}

void Izquierda::exec()
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

AttrList Izquierda::attrList()
{
	auto attr=AB::Node::attrList();
	attr.push_back("code");
	attr.push_back("nombre");
	attr.push_back("code_default");
	return attr;
}

Object Izquierda::attr(const std::string& name)
{
	if (name == "nombre")
	{
		return to_object(nombre);
	}
	else if(name == "code_default"){
		return to_object(code_default);
	}
	else //if(name=="code"){
		return to_object(code);
	
}

void Izquierda::setAttr(const std::string& name, Object obj)
{
	if (name=="code"){
		code=object2string(obj);
		Py_XDECREF(compiled_code);
		compiled_code=Py_CompileString(code.c_str(), this->name().c_str(), Py_file_input);
		if (!compiled_code)
			PyErr_Print();
	}
	else if (name == "nombre")
	{
		nombre=object2string(obj);
	}
	else if( name =="code_default"){
		code_default=object2string(obj);
	}
	else
		return Action::setAttr(name, obj);
}

void Izquierda::setManager(Manager* manager)
{
	AB::Node::setManager(manager);
	AB::Interfaz::ab_module_manager=manager;
}
