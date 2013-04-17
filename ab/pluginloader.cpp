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

#include "pluginloader.h"
#include "plugin.hpp"
#include "log.h"

#include <dirent.h>
#include <sys/stat.h>
#include <dlfcn.h>

using namespace AB;

/// from http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
static bool endswith(std::string const &fullString, std::string const &ending){
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}

static bool isdir(const std::string &filename){
	struct stat st;
	lstat(filename.c_str(),&st);
	return S_ISDIR(st.st_mode);
}

void PluginLoader::loadPath(const std::string& path)
{
	//DEBUG("Load plugins at %s",path.c_str());
	DIR *dir=opendir(path.c_str());
	if (!dir){
		ERROR("%s is not a directory to load plugins", path.c_str());
		return;
	}
	struct dirent *st;
	while( (st=readdir(dir)) != NULL ){
		if (st->d_name[0]=='.') // Skip hidden
			continue;
		//DEBUG("Check %s",st->d_name);
		std::string fullname=path+"/"+st->d_name;
		if (endswith(fullname, ".so")){
			loadPlugin(fullname);
		}
		if (isdir(fullname)){
			loadPath(fullname);
		}
	}
	
	closedir(dir);
}

bool PluginLoader::loadPlugin(const std::string& filename)
{
	void *dl=dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	bool inuse=false;
	
	// Trick to compiler to allow void* to ab_init_f conversion.
	union{
		ab_init_f *init;
		void *init_v;
	};
	init_v=dlsym(dl, "ab_init");
	
	if (!init) // Check also C++ name
		init_v=dlsym(dl, "_Z7ab_initv");
	
	if (init){
		inuse=true;
		init();
	}
	
	if (!inuse)
		dlclose(dl);
	else{
		INFO("Loaded plugin at %s", filename.c_str());
	}
	return false;
}
