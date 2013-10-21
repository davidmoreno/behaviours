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

#include <onion/onion.hpp>
#include <onion/url.hpp>
#include <onion/extrahandlers.hpp>

extern "C"{
  #include <onion/handlers/webdav.h>
}
#include <ab/manager.h>
#include <ab/lua.h>
#include <ab/log.h>
#include <ab/factory.h>
#include <ab/events/start.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "nodemanager.hpp"
#include "statichandler.hpp"
#include "browsefiles.hpp"


namespace AB{
	std::string static_dir=AB_PREFIX "/share/ab/static/";
	std::string data_dir=std::string(getenv("HOME"))+"/behaviours/";
}

using namespace ABServer;
using namespace std;
using namespace AB;

Onion::Onion *o;
 
std::shared_ptr<AB::Manager> manager;
std::shared_ptr<NodeManager> nodeManager;
  
static void node_notify_enter(AB::Node *n) {
  nodeManager->activateNode(n);
}

static void node_notify_exit(AB::Node *n) {
  nodeManager->deactivateNode(n);
}

static void on_SIGINT(int){
	static bool twice=false;
	if (twice){
		INFO("Exiting on second SIGINT.");
		exit(1);
	}
	twice=true;
	o->listenStop();
}

int main(void){
	if (getenv("ABPATH")){
		WARNING("Setting default path to %s", getenv("ABPATH"));
		static_dir=std::string(getenv("ABPATH")) + "/static/";
	}
	
	{
		struct stat sb;
		if (stat(static_dir.c_str(), &sb)<0){
			ERROR("Can't open data dir (%s). Set it with environment variable DIAPATH to the parent "
						"of static and data dirs. Alternatively run dia at the source directory of dia.",
				 static_dir.c_str());
			exit(1);
		}
	}
	
	manager=make_shared<Manager>();
	nodeManager=make_shared<NodeManager>(manager);
	BrowseFiles browser("/");
	
	
	ABServer::init();
	//DIA::init(&manager);

	AB::manager_notify_node_enter = node_notify_enter;
	AB::manager_notify_node_exit = node_notify_exit;
	
	manager->loadBehaviour(data_dir + "current.ab");
	o=new Onion::Onion(O_POOL);
	
	signal(SIGINT, on_SIGINT);
	signal(SIGTERM, on_SIGINT);
	
	o->setHostname("0.0.0.0");
	o->setPort("8081");
	
	Onion::Url url(o);
	
	url.add("", new Onion::RedirectHandler("/static/index.html"));
	url.add("^manager/", nodeManager.get(), &NodeManager::manager);
	url.add("^node/", nodeManager.get(), &NodeManager::node);
	url.add("^connections/", nodeManager.get(), &NodeManager::connections);
	url.add("^lua/", nodeManager.get(), &NodeManager::lua);
	url.add("^upload/", nodeManager.get(), &NodeManager::uploadXML);
	url.add("^wavload/", nodeManager.get(), &NodeManager::uploadWAV);
	url.add("^static/", new StaticHandler(static_dir));
	url.add("^data/browse", &browser);
	url.add("^data/", nodeManager.get(), &NodeManager::save);
	url.add("^events", nodeManager.get(), &NodeManager::events);
	
	ONION_DEBUG("Webdav dir at %s", data_dir.c_str());
	onion_handler *w=onion_handler_webdav(data_dir.c_str(),NULL);
	onion_url_add_handler(url.c_handler(), "^webdav/?", w);
	
	//nodeManager.mimeFill();

	INFO("Listening at 127.0.0.1:8081");
	
	o->listen();
	
	delete o;
	std::cout<<"closed"<<endl;
}

