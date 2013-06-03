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

/**
 * @short Loads paths and looks for plugins there.
 * 
 * For each path given, it looks for all .so files (recursively), and 
 * tries to load it looking for the plugin interface markers.
 */
namespace AB{
	class PluginLoader{
	public:
		static void loadPath(const std::string &path);
		static bool loadPlugin(const std::string &filename);
	};
}