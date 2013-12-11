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
#include "manager.h"

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

static bool disabledPluginByName(const std::string &fullname, std::set<std::string> &disabled){
	std::string name;
	name=fullname.substr(fullname.rfind("/lib")+4);
	if (endswith(name,".so"))
		name=name.substr(0,name.length()-3);
	auto f=disabled.find(name);
	return f!=std::end(disabled);
}

void PluginLoader::loadPath(const std::string& path, Manager *manager)
{
	//DEBUG("Load plugins at %s",path.c_str());
	DIR *dir=opendir(path.c_str());
	if (!dir){
		ERROR("%s is not a directory to load plugins", path.c_str());
		return;
	}
	auto disabled=manager->settings.getSet("plugins.disabled");
	struct dirent *st;
	while( (st=readdir(dir)) != NULL ){
		if (st->d_name[0]=='.') // Skip hidden
			continue;
		DEBUG("Check %s",st->d_name);
		std::string fullname=path+"/"+st->d_name;
		if (endswith(fullname, ".so")){
			if (!disabledPluginByName(fullname, disabled))
				loadPlugin(fullname, manager);
			else{
				DEBUG("Plugin disabled: %s", fullname.c_str());
			}
		}
		else if (isdir(fullname)){
			if (!disabledPluginByName(fullname, disabled))
				loadPath(fullname, manager);
			else{
				DEBUG("Plugin path disabled: %s", fullname.c_str());
			}
		}
	}
	
	closedir(dir);
}

bool PluginLoader::loadPlugin(const std::string& filename, Manager *manager)
{
	void *dl=dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	bool inuse=false;
	
	if (!dl){
		WARNING("Plugin file at %s could not be loaded: %s", filename.c_str(), dlerror());
		return false;
	}
	
	// Trick to compiler to allow void* to ab_init_f conversion.
	union{
		ab_init_f *init;
		void *init_v;
		ab_init_w_manager_f *init_wm;
	};
	char type='v'; // By default type void.
	init_v=dlsym(dl, "ab_init");
	
	if (!init) // Check also C++ name
		init_v=dlsym(dl, "_Z7ab_initv");
	
	if (!init){
		init_v=dlsym(dl, "_Z7ab_initPN2AB7ManagerE");
		if (init)
			type='m'; // It gets the manager object.
	}
	
	if (!init){
		WARNING("Plugin file at %s could not be loaded: %s", filename.c_str(), dlerror());
		return false;
	}
	
	if (init){
		INFO("Loaded plugin at %s", filename.c_str());
		inuse=true;
		try{
			if (type=='m')
				init_wm(manager);
			else 
				init();
		}
		catch(const std::exception &e){
			ERROR("Error loading plugin %s: %s", filename.c_str(), e.what());
		}
	}
	
	if (!inuse){
		dlclose(dl);
		return false;
	}
	return true;
}
