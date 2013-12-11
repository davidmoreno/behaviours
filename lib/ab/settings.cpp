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

#include <fstream>
#include <map>

#include "settings.hpp"
#include "log.h"

using namespace AB;

namespace AB{
	static std::string trim(const std::string &s){
		auto start=s.find_first_not_of(" ");
		if (start==s.npos)
			return s;
		auto end=s.find_last_not_of(" ");
		return s.substr(start, 1+end-start);
	}
	static std::string nocomments(const std::string &s){
		auto h=s.find_first_of("#");
		if (h==s.npos)
			return s;
		return s.substr(0,h);
	}
	
	class IniFile{
		std::map<std::string, std::map<std::string, std::string>> data;
	public:
		IniFile(const std::string &_filename){
			std::string filename=_filename;
			if (_filename[0]=='~'){
				filename=getenv("HOME")+_filename.substr(1);
			}
			DEBUG("Full filename is %s", filename.c_str());
			std::ifstream ini(filename);
			if (!ini.is_open()){
				DEBUG("Could not open config file %s", filename.c_str());
				return;
			}
			std::string current_section;
			std::string line;
			std::string key;
			std::string value;
			char tmp[1024];
			int linenr=0;
			while(!ini.eof()){
				ini.getline(tmp, sizeof(tmp));
				linenr++;
				line=trim(nocomments(tmp));
				if (line.length()==0)
					continue;
				//DEBUG("isglobal %s %d %d", line.c_str(), line[0]=='[' , line[line.length()-1]==']');
				if (line[0]=='[' && line[line.length()-1]==']'){
					current_section=line.substr(1, line.length()-2);
					DEBUG("Set current section to %s", current_section.c_str());
				}
				else{
					auto eq=line.find("=");
					if (eq==line.npos){
						ERROR("Invalid line at config file %s, line %d", filename.c_str(), linenr);
					}
					else{
						key=line.substr(0,eq);
						value=line.substr(eq+1);
						
						DEBUG("Add %s.%s=%s", current_section.c_str(), key.c_str(), value.c_str());
						data[current_section][key]=value;
					}
				}
			}
		}
		std::string get(const std::string &k){
			auto dot=k.find('.');
			std::string section=k.substr(0,dot);
			std::string key=k.substr(dot+1);
			DEBUG("Get <%s>.<%s> = %s", section.c_str(), key.c_str(), data[section][key].c_str()); 
			return data[section][key];
		}
	};
};

Settings::Settings(const std::string& filename) : settings(std::make_shared<IniFile>(filename))
{

}

std::string Settings::get(const std::string& key)
{
	return settings->get(key);
}

std::string Settings::get(const std::string &key, const std::string &defvalue){
	auto ret=settings->get(key);
	if (ret.empty())
		return defvalue;
	return ret;
}

std::set< std::string > Settings::getSet(const std::string& key)
{
	DEBUG("Get set %s", key.c_str());
	std::string data=settings->get(key);
	std::set<std::string> ret;
	auto lastc=-1;
	auto nextc=data.find(',');
	while(nextc!=data.npos){
		ret.insert(data.substr(lastc+1, nextc-lastc-1));
		
		lastc=nextc;
		nextc=data.find(',', lastc+1);
	}
	if(!data.empty())
		ret.insert(data.substr(lastc+1));
	
	for(auto c:ret)
		DEBUG("Added %s", c.c_str());
	return ret;
}
