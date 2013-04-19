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

#include <sys/stat.h>
#include <dirent.h>
#include <onion/log.h>

#include "utils.hpp"

namespace AB{

bool isdir(const std::string &filename){
	struct stat st;
	lstat(filename.c_str(),&st);
	return S_ISDIR(st.st_mode);
}

/// from http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
bool endswith(std::string const &fullString, std::string const &ending){
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
	} else {
		return false;
	}
}


std::vector<std::string> getSubdirectories(const std::string& basepath){
	std::vector<std::string> paths;
	DIR *dir=opendir(basepath.c_str());
	if (!dir){
		ONION_ERROR("Cant open %s directory for static content", basepath.c_str());
		throw std::exception(); // FIXME, throw a better exception.
	}
	
	struct dirent *ent;
	while ( (ent=readdir(dir)) ){
		if (ent->d_name[0]!='.')
			if (isdir(basepath + "/" + ent->d_name))
				paths.push_back(ent->d_name);
	}
	closedir(dir);
	return paths;
}

};
