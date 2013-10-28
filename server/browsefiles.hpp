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

#include <string>

#include <onion/onion.hpp>

namespace ABServer{

	/**
	 * @short Simple handler that allows to browse over a predefined directory.
	 * 
	 * Simple browsing just means check the contents of the directories. Should use another methods
	 * to save or load data from it.
	 * 
	 * Browsing is made throw the operator() using json.
	 */
	class BrowseFiles : public Onion::Handler{
		std::string basedir;
	public:
		BrowseFiles(const std::string &basedir);
		
		/**
		 * @short Receives the path query and returns a JSON with the contents of that path:
		 * 
		 *   { "files": [ { "filename": "current.ab", "type": "file" }, { "filename": "other", "type": "dir" } ] }
		 */
		virtual onion_connection_status operator()(Onion::Request& , Onion::Response& );
		
	private:
		std::string get_real_dir(const std::string &subdir);
	};
}
