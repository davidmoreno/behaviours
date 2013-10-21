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

#include <onion/request.hpp>
#include <onion/response.hpp>
#include <dirent.h>
#include <json/json.h>

#include "browsefiles.hpp"

using namespace ABServer;

BrowseFiles::BrowseFiles(const std::string& basedir) : basedir(basedir)
{

}

onion_connection_status BrowseFiles::operator()(Onion::Request& req, Onion::Response& res)
{
	auto path=get_real_dir(req.query().get("path"));
	ONION_DEBUG("Get data from path %s",path.c_str());
	
	json_object *jobj = json_object_new_object();
	json_object *files = json_object_new_array();
	json_object_object_add(jobj,"files",files);
	
	DIR *dir=opendir(path.c_str());
	if (!dir)
		return OCS_INTERNAL_ERROR;
	dirent *ent;
	while ( (ent=readdir(dir)) ){
		if (ent->d_name[0]=='.') // Do not show hidden
			continue;
		json_object *file=json_object_new_object();
		json_object_object_add(file, "filename", json_object_new_string(ent->d_name));
		json_object_object_add(file, "type", json_object_new_string( (DT_DIR==ent->d_type) ? "dir" : "file" ));
		
		json_object_array_add(files, file);
	}
	
	res<<json_object_get_string(jobj);
	json_object_put(jobj);
	
	return OCS_PROCESSED;
}


std::string BrowseFiles::get_real_dir(const std::string& subdir){
	if (subdir.find("..")!=std::string::npos)
		throw(Onion::HttpInternalError("Cant do .. on path on browse files: "+subdir));
	
	return basedir+subdir;
}
