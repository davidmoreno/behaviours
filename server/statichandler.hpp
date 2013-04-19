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

#pragma once

#include <onion/onion.hpp>
#include <vector>

namespace AB{
	/**
	* @short Serves static content, but content can be overwritten by priority.
	* 
	* It sets a base directory, and for each subdirectory, in reverse alphabetical order, looks
	* for the first that have the requested file. That file is returned to the client.
	* 
	* This way higher priority content will "overwrite" lower priority content, for example to 
	* allow plugins to overwrite the base style.
	*/
	class StaticHandler : public Onion::Handler{
		std::string basepath;
		std::vector<Onion::Handler*> handlers;
	public:
		StaticHandler(const std::string &basepath);
    virtual ~StaticHandler();
		virtual onion_connection_status operator()(Onion::Request &req, Onion::Response &res);
	};
}
