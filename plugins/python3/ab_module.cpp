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
#include <ab/log.h>
#include <ab/object_basic.h>

namespace AB{
namespace Python3{

	extern Manager *ab_module_manager;
	static PyObject *manager_error;

	typedef struct{
		PyObject_HEAD
		Node *node;
	} NodeObject;

	static PyTypeObject NodeObjectType = {
			PyVarObject_HEAD_INIT(NULL,0)
			"ab.node",             /*tp_name*/
			sizeof(NodeObject), /*tp_basicsize*/
			0,                         /*tp_itemsize*/
			0,                         /*tp_dealloc*/
			0,                         /*tp_print*/
			0,                         /*tp_getattr*/
			0,                         /*tp_setattr*/
			0,                         /*tp_compare*/
			0,                         /*tp_repr*/
			0,                         /*tp_as_number*/
			0,                         /*tp_as_sequence*/
			0,                         /*tp_as_mapping*/
			0,                         /*tp_hash */
			0,                         /*tp_call*/
			0,                         /*tp_str*/
			0,                         /*tp_getattro*/
			0,                         /*tp_setattro*/
			0,                         /*tp_as_buffer*/
			Py_TPFLAGS_DEFAULT,        /*tp_flags*/
			"Node objects",           /* tp_doc */
	};


	
	static PyObject *PyObject_new(AB::Object &obj){
		auto t=obj->type();
		if (t==AB::Integer::type)
				return PyLong_FromLong(object2int(obj));
		if (t==AB::Float::type)
				return PyFloat_FromDouble(object2float(obj));
		if (t==AB::String::type)
				return PyUnicode_FromString(object2string(obj).c_str());
		
		try{
			NodeObject *no=PyObject_New(NodeObject, &NodeObjectType);
			no->node=object2node(obj);
			return (PyObject*)(no);
		}
		catch(const AB::object_not_convertible &exc){
		}
		
		
		DEBUG("Create node %s failed", obj->type());
		return NULL;
	}

	/// Define a new class for the manager. 
	static PyObject *ab_manager_resolve(PyObject *self, PyObject *args){
		const char *name;
		
		if (!PyArg_ParseTuple(args, "s", &name)){
			return NULL;
		}
		
		try{
			AB::Object n( ab_module_manager->resolve(name) );
			return PyObject_new(n);
		}
		catch(const AB::attribute_not_found &e){
			PyErr_SetString(manager_error, (std::string("Could not resolve symbol ")+name).c_str());
			return NULL;
		}
	}

	static PyMethodDef ab_methods[] = {
		{"resolve", ab_manager_resolve, METH_VARARGS, NULL},
		{NULL, NULL, 0, NULL}
	};

	static struct PyModuleDef ab_module = {
		PyModuleDef_HEAD_INIT,
		"ab",
		"Behaviours interface module",
		-1,
		ab_methods
	};

	PyObject *PyInit_ab(void){
		auto m = PyModule_Create(&ab_module);
		if (!m)
			return NULL;
		manager_error = PyErr_NewException("ab.exception", NULL, NULL);
		Py_INCREF(manager_error);
		PyModule_AddObject(m, "exception", manager_error);
		
		NodeObjectType.tp_new = PyType_GenericNew;
		if (PyType_Ready(&NodeObjectType) < 0)
			return NULL;

		//Py_INCREF(&NodeObjectType);
		//PyModule_AddObject(m, "Node", (PyObject*)&NodeObjectType);
		
		return m;
	}
}
}
