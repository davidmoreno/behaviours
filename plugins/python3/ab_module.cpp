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
	static PyObject *object2pyobject(AB::Object &obj);
	static Object to_object(PyObject *obj);
	
	extern Manager *ab_module_manager;
	static PyObject *manager_error;

	typedef struct{
		PyObject_HEAD
		Node *node;
	} NodeObject;
	

	static PyObject *node_getattr(PyObject *self, char *attr){
		NodeObject *oself=(NodeObject*)self;
		try{
			auto v=oself->node->attr(attr);
			return object2pyobject(v);
		}
		catch(const AB::attribute_not_found &a){
			return NULL;
		}
	}
	static int node_setattr(PyObject *self, char *attr, PyObject *value){
		NodeObject *oself=(NodeObject*)self;
		try{
			oself->node->setAttr(attr, to_object(value));
			return 0;
		}
		catch(const AB::attribute_not_found &a){
			return -1;
		}
	}
	
	static PyObject *node_dir(PyObject *self){
		NodeObject *oself=(NodeObject*)self;
		try{
			auto attrlist=oself->node->attrList();
			auto I=attrlist.begin(), endI=attrlist.end();
			PyObject *ret=PyList_New(attrlist.size());
			Py_INCREF(ret);
			int i;
			for(i=0;I!=endI;++i,++I){
				PyObject *name=PyUnicode_FromString((*I).c_str());
				Py_INCREF(name);
				PyList_SetItem(ret, i, name);
			}
			return ret;
		}
		catch(const AB::attribute_not_found &a){
			return NULL;
		}
	}
	
	static PyTypeObject NodeObjectType = {
			PyVarObject_HEAD_INIT(NULL,0)
			"ab.node",             /*tp_name*/
			sizeof(NodeObject), /*tp_basicsize*/
			0,                         /*tp_itemsize*/
			0,                         /*tp_dealloc*/
			0,                         /*tp_print*/
			node_getattr,                         /*tp_getattr*/
			node_setattr,                         /*tp_setattr*/
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


	static PyObject *object2pyobject(AB::Object &obj){
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
	
	static Object to_object(PyObject *obj){
		if (PyBool_Check(obj))
			return AB::to_object( obj == Py_True );
		if (PyFloat_Check(obj))
			return AB::to_object( PyFloat_AsDouble(obj) );
		if (PyLong_Check(obj))
			return AB::to_object( (int)PyLong_AsLong(obj) );
		if (PyUnicode_Check(obj))
			return AB::to_object( PyUnicode_AsUTF8(obj) );
		
		throw(AB::object_not_convertible( obj->ob_type->tp_name, "Object"));
	}

	/// Define a new class for the manager. 
	static PyObject *ab_manager_resolve(PyObject *self, PyObject *args){
		const char *name;
		
		if (!PyArg_ParseTuple(args, "s", &name)){
			return NULL;
		}
		
		try{
			AB::Object n( ab_module_manager->resolve(name) );
			return object2pyobject(n);
		}
		catch(const AB::attribute_not_found &e){
			PyErr_SetString(manager_error, (std::string("Could not resolve symbol ")+name).c_str());
			return NULL;
		}
	}

	static PyObject *ab_manager_list_nodes(PyObject *self){
		auto nodelist=ab_module_manager->getNodes();
		auto typelist=AB::Factory::list();
		PyObject *ret=PyList_New(nodelist.size() + typelist.size());
		Py_INCREF(ret);
		{
			auto I=nodelist.begin(), endI=nodelist.end();
			int i;
			for(i=0;I!=endI;++i,++I){
				PyObject *name=PyUnicode_FromString((*I)->name().c_str());
				Py_INCREF(name);
				PyList_SetItem(ret, i, name);
			}
		}
		int b=nodelist.size();
		{
			auto I=typelist.begin(), endI=typelist.end();
			int i;
			for(i=0;I!=endI;++i,++I){
				PyObject *name=PyUnicode_FromString((*I).c_str());
				Py_INCREF(name);
				PyList_SetItem(ret, b + i, name);
			}
		}
		return ret;
	}
	
	static PyMethodDef ab_methods[] = {
		{"resolve", ab_manager_resolve, METH_VARARGS, NULL},
		{"list_nodes", (PyCFunction)ab_manager_list_nodes, METH_NOARGS, NULL},
		{NULL, NULL, 0, NULL}
	};

	static struct PyModuleDef ab_module = {
		PyModuleDef_HEAD_INIT,
		"ab",
		"Behaviours interface module",
		-1,
		ab_methods
	};

	static struct PyMethodDef node_methods[] = {
		{ "__dir__", (PyCFunction)node_dir, METH_NOARGS, NULL},
		{NULL, NULL, 0, NULL}
	};
	
	PyObject *PyInit_ab(void){
		auto m = PyModule_Create(&ab_module);
		if (!m)
			return NULL;
		manager_error = PyErr_NewException("ab.exception", NULL, NULL);
		Py_INCREF(manager_error);
		PyModule_AddObject(m, "exception", manager_error);
		
		NodeObjectType.tp_new = PyType_GenericNew;
		NodeObjectType.tp_methods = node_methods;
		if (PyType_Ready(&NodeObjectType) < 0)
			return NULL;

		//Py_INCREF(&NodeObjectType);
		//PyModule_AddObject(m, "Node", (PyObject*)&NodeObjectType);
		
		return m;
	}
}
}
