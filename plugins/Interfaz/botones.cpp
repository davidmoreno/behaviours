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

#include "botones.hpp"
#include <ab/plugin.hpp>
#include <ab/factory.h>
#include <ab/log.h>

using namespace AB;

Botones::Botones(const char* type): Action(type)
{
	arriba = "";
	abajo = "";
	izquierda = "";
	derecha = "";
	nombre = "Botones";
}


void Botones::setAttr(const std::string &k, AB::Object s)
{
	if(k == "arriba"){
	  arriba=object2string(s);
	}
	else if (k == "abajo")
	{
		abajo=object2string(s);
	}else if (k == "izquierda")
	{
		izquierda=object2string(s);
	}else if (k == "derecha")
	{
		derecha=object2string(s);
	}else if (k == "nombre")
	{
		nombre=object2string(s);
	}
	else Action::setAttr(k,s);
}

AB::Object Botones::attr(const std::string &k)
{
	if (k == "arriba")
	{
		return to_object(arriba);
	}
	else if (k == "abajo")
	{
		return to_object(abajo);
	}
	else if (k == "izquierda")
	{
		return to_object(izquierda);
	}
   	else if (k == "derecha")
	{
		return to_object(derecha);
	}else if (k == "nombre")
	{
		return to_object(nombre);
	}
	else return Action::attr(k);
}

AB::AttrList Botones::attrList()
{
  AB::AttrList l;
  l.push_back("arriba");
  l.push_back("abajo");
  l.push_back("izquierda");
  l.push_back("derecha");
  l.push_back("nombre");
  return l;
}


void Botones::exec()
{
	INFO("Exec botones %p",this);
}
