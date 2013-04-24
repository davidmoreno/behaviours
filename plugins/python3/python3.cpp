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
	namespace Python3{
		PyObject *PyInit_ab(void);
		Manager *ab_module_manager=NULL;
	}
}



void ab_init(void){
	DEBUG("Loaded python3 plugin");
	AB::Factory::registerClass<AB::Python3Action>("python3action");
}

using namespace AB;
using namespace AB::Python3;

static int python_ref_count=0;

Python3Action::Python3Action(const char* type): Action(type)
{
	if (python_ref_count==0){
		PyImport_AppendInittab("ab",PyInit_ab);
		Py_SetProgramName((wchar_t*)("behaviours"));
		Py_Initialize();
	}
	python_ref_count++;
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
	PyRun_SimpleString( code.c_str() );
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
