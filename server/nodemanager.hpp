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

#ifndef __ABSERVER_NODE_MANAGER_HPP__
#define __ABSERVER_NODE_MANAGER_HPP__

#include <sys/time.h>

#include <onion/types.h>
#include <onion/onion.hpp>
#include <onion/dict.hpp>

#include <ab/node.h>

#include <thread>
#include <memory>

const size_t max_queue_size = 25;
namespace AB{
	class Manager;
	class Node;
	
	extern std::string static_dir;
	extern std::string data_dir;
};

namespace ABServer{
  
	void init();
	/**
	 * @short Serves JSON data with all available nodes, and more info.
	 */
	class NodeManager{
		std::shared_ptr<AB::Manager> ab;
		std::shared_ptr<std::thread> abthread;
		bool running;

		std::string current_ab_file;

		std::string downloaded;
		struct timeval lastAutosave;
		bool needsAutosave, forceUpdate;

		void exec_behaviour_thread();
	public:
		NodeManager(std::shared_ptr<AB::Manager> &ab);
		
		Onion::Dict attributesOf(AB::Node::p node);
		
		onion_connection_status manager(Onion::Request &req, Onion::Response &res);
		onion_connection_status lua(Onion::Request &req, Onion::Response &res);
		onion_connection_status node(Onion::Request &req, Onion::Response &res);
		onion_connection_status connections(Onion::Request &req, Onion::Response &res);
		onion_connection_status update(Onion::Request &req, Onion::Response &res);
		onion_connection_status uploadXML(Onion::Request &req, Onion::Response &res);
		onion_connection_status uploadWAV(Onion::Request &req, Onion::Response &res);
		onion_connection_status save(Onion::Request &req, Onion::Response &res);
		onion_connection_status events(Onion::Request &req, Onion::Response &res);

		void exec_behaviour();
		
		void activateNode(AB::Node::p n);
		void deactivateNode(AB::Node::p n);
	private:
		onion_connection_status list_xml_files(Onion::Request &req, Onion::Response &res);
		
	};
}

#endif
