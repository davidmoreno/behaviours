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

#include <onion/extrahandlers.hpp>
#include <onion/log.h>
#include <dirent.h>
#include <algorithm>
#include <sys/stat.h>

#include "utils.hpp"
#include "statichandler.hpp"


using namespace AB;

bool path_cmp(const std::string &a, const std::string &b){
	return a.compare(b)>0;
}

StaticHandler::StaticHandler(const std::string& basepath) : basepath(basepath)
{
	ONION_DEBUG("Look for static data at %s", basepath.c_str());
	std::vector<std::string> paths=getSubdirectories(basepath);
	
	if (paths.size()==0){
		ONION_ERROR("No static data found at %s", basepath.c_str());
		throw std::exception(); // FIXME, throw a better exception.
	}
	
	std::sort(paths.begin(), paths.end(), path_cmp);
	
	auto I=paths.begin(), endI=paths.end();
	for(;I!=endI;++I){
		ONION_DEBUG("%s",I->c_str());
		handlers.push_back(new Onion::StaticHandler(basepath+"/"+*I+"/"));
	}
}

StaticHandler::~StaticHandler()
{
	auto I=handlers.begin(), endI=handlers.end();
	for(;I!=endI;++I){
		delete *I;
	}
}


onion_connection_status StaticHandler::operator()(Onion::Request& req, Onion::Response& res)
{
	auto I=handlers.begin(), endI=handlers.end();
	for(;I!=endI;++I){
		onion_connection_status st=(**I)(req, res);
		if (st==OCS_PROCESSED)
			return st;
	}
	return OCS_NOT_PROCESSED;
}
