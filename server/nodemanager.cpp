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

#include <onion/response.hpp>
#include <onion/request.hpp>
#include <onion/shortcuts.h>
extern "C"{
#include <onion/mime.h>
}

#include <algorithm>
#include <set>
#include <ab/manager.h>
#include <ab/node.h>
#include <ab/event.h>
#include <ab/factory.h>
#include <ab/connection.h>
#include <ab/log.h>
#include <ab/builtin_nodes.hpp>

#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

#include "utils.hpp"
#include "nodemanager.hpp"
#include "../lib/ab/manager.h"
#include "../lib/ab/node.h"

using namespace ABServer;
using namespace AB;

void ABServer::init()
{  
  AB::registerBuiltinNodes();
}

//std::mutex NodeManager::lua_mutex;
//std::mutex NodeManager::activeNodes_mutex;
//std::mutex NodeManager::inactiveNodes_mutex;

NodeManager::NodeManager(std::shared_ptr<AB::Manager> &ab) : ab(ab)
{
	running=false;
  abthread=NULL;
  downloaded = "";
  needsAutosave = false;
  forceUpdate = false;
	current_ab_file=data_dir+"/current.ab";
  image_dir= data_dir+"static/";
  image_dir_2=data_dir+"static/image/";
  save_ab_file="";
	mkdir(data_dir.c_str(),0777);
  mkdir(image_dir.c_str(),0777);
  mkdir(image_dir_2.c_str(),0777);
  gettimeofday(&lastAutosave, NULL);
}


Onion::Dict NodeManager::attributesOf(AB::Node::p node)
{
  Onion::Dict nodeData;

  AB::AttrList al=node->attrList();
  AB::AttrList::iterator J=al.begin(), endJ=al.end();
  for(;J!=endJ;++J){
      try{
        nodeData.add(*J, object2string(node->attr(*J)));
      }
      catch(const AB::attribute_not_found &){
        nodeData.add(*J,"unknown", OD_DUP_KEY);
      }
    }

  return nodeData;
}

void NodeManager::exec_behaviour_thread()
{
	running=true;
	ab->exec();
	running=false;
}


void NodeManager::exec_behaviour()
{
	if (running)
		return;
	if (abthread.get()){
		try{
			abthread->join();
		}
		catch(...){
		}
	}
	abthread=std::make_shared<std::thread>(&NodeManager::exec_behaviour_thread, this);
}


onion_connection_status NodeManager::manager(Onion::Request& req, Onion::Response& res)
{
  Onion::Dict post=req.post();
  if (post.has("run")){
      ab->saveBehaviour(current_ab_file);
			exec_behaviour();
			res<<"OK";
			return OCS_PROCESSED;
  } else if (post.has("stop")) {
    if (abthread.get()){
      ab->cancel();
			res<<"OK";
			return OCS_PROCESSED;
    }
  } else if (post.has("save")) {
		ab->saveBehaviour(current_ab_file);
		res<<"OK";
		return OCS_PROCESSED;
  } else if(post.has("clear")) {
    ab->clear();
    res<<"OK";
    return OCS_PROCESSED;

  } else if(post.has("description")) {
    ab->setName(req.post().get("name"));
    ab->setDescription(req.post().get("description"));
    ab->saveBehaviour(current_ab_file);
    res<<"OK";
    return OCS_PROCESSED;

  } else if(post.has("viewpoint")) {
    ab->setViewpoint(req.post().get("viewpoint"));
    res<<"OK";
    return OCS_PROCESSED;

  } else if(post.has("filename")) {
    downloaded = post.get("filename");
    WARNING("Got filename: %s", downloaded.c_str());
    res<<"OK";
    return OCS_PROCESSED;
  }

  if(req.query().has("refresh")) {
    ab->saveBehaviour(current_ab_file);
    return onion_shortcut_response_file(current_ab_file.c_str(), req.c_handler(), res.c_handler());
  }

  return OCS_NOT_PROCESSED;
}

onion_connection_status NodeManager::save(Onion::Request &req, Onion::Response &res){
 Onion::Dict post=req.post();
  if (post.count()){
      save_ab_file=post.get("save");
      if(save_ab_file!="null"){
        save_ab_file=save_ab_file.substr(1,save_ab_file.size());
        WARNING("%s", save_ab_file.c_str());
        ab->saveBehaviour(save_ab_file);
      }
  }
  
  ab->saveBehaviour(current_ab_file);
	res.setHeader("Content-Disposition", "attachment; filename="+req.path());
	return onion_shortcut_response_file(current_ab_file.c_str(),req.c_handler(), res.c_handler());
}

onion_connection_status NodeManager::node(Onion::Request& req, Onion::Response& res)
{
  if (req.path()!=""){
      AB::Node::p node=ab->getNode(req.path());
      if (!node){
          throw(Onion::HttpInternalError("Cant find node "+req.path()));
        }
      if (req.post().count()){
          if (req.post().has("connect_to")){
              AB::Node::p nodeB=ab->getNode(req.post().get("connect_to"));
              AB::Connection::p c=ab->connect(node, nodeB);
              if(!c) {
                  if (!nodeB){
                      throw(Onion::HttpInternalError("Can't find node "+req.post().get("connect_to")));
                    }else if(std::dynamic_pointer_cast<AB::Event>(nodeB)){
                      throw(Onion::HttpInternalError("Can't connect to node "+req.post().get("connect_to")+" because it is an Event"));
                    } else throw(Onion::HttpInternalError("Can't connect to node "+req.post().get("connect_to")));
                }

              if (req.post().has("guard")){
                  c->setGuard(req.post().get("guard"));
                }
              res<<c->name();
              return OCS_PROCESSED;
            }
          if (req.post().has("disconnect_from")){
              AB::Node::p nodeB=ab->getNode(req.post().get("disconnect_from"));
              if (!nodeB){
                  throw(Onion::HttpInternalError("Cant find node "+req.post().get("connect_to")));
                }
              ab->disconnect(node, nodeB);
              res<<"OK";
              return OCS_PROCESSED;
            }
          if (req.post().has("guard") && req.post().has("to")){
              WARNING("Got guard:%s and to:%s",req.post().get("guard").c_str(),req.post().get("to").c_str());
              AB::Node::p nodeB=ab->getNode(req.post().get("to"));
              if (!nodeB){
                  throw(Onion::HttpInternalError("Cant find node "+req.post().get("to")));
                }
              AB::Connection::p c=ab->getConnection(node,nodeB);
              c->setGuard(req.post().get("guard"));
              res<<c->guard();
              return OCS_PROCESSED;
            }
          if (req.post().has("attr")){
              std::string key=req.post().get("attr");
              std::string value=req.post().get("value");

              try{
                node->setAttr(key, AB::to_object(value));
              }
              catch(const std::exception &e){
                res.setCode(HTTP_INTERNAL_ERROR);
                res<<e.what();
                return OCS_PROCESSED;
              }
              res<<"OK";
              return OCS_PROCESSED;
            }
          if (req.post().has("notify")){
              ab->notify(node);
              res<<"OK";
              return OCS_PROCESSED;
            }
          if (req.post().has("x")){
              node->setPosition(atof(req.post().get("x").c_str()),atof(req.post().get("y").c_str()));
              res<<"OK";
              return OCS_PROCESSED;
            }
          if (req.post().has("remove")){
              ab->deleteNode(node);
              res<<"OK";
              return OCS_PROCESSED;
            }
          if (req.post().has("name")){
			  node->setName(req.post().get("name"));
			  res<<"OK";
			  return OCS_PROCESSED;
		  }
          return OCS_INTERNAL_ERROR;
        }
      Onion::Dict d=attributesOf(node);
      return onion_shortcut_response_json(d.c_handler(), req.c_handler(), res.c_handler());
    }


  if (req.post().has("create_node")){
      AB::Node::p n=AB::Factory::createNode(req.post().get("create_node"));
      ab->addNode(n);
      res<<n->name();
      return OCS_PROCESSED;
    }
  if (req.query().has("list_types")){
      std::vector<std::string> l=AB::Factory::list();
      res<<"[";
      std::vector<std::string>::iterator I=l.begin(), endI=l.end();
      for(;I!=endI;){
          res<<'"'<<*I<<'"';
          ++I;
          if (I!=endI)
            res<<",";
        }
      res<<"]";
      return OCS_PROCESSED;
    }
  if (req.query().has("list_files")){
    return list_xml_files(req, res);
    }

  else{
      const std::set<AB::Node::p> nodes=ab->getNodes();

      Onion::Dict d;
      std::set<AB::Node::p>::const_iterator I=nodes.begin(), endI=nodes.end();
      for (;I!=endI;++I){
          Onion::Dict nodeData=attributesOf(*I);

          d.add((*I)->name(),nodeData);
        }

      return onion_shortcut_response_json(d.c_handler(), req.c_handler(), res.c_handler());
    }
}


onion_connection_status NodeManager::events(Onion::Request& req, Onion::Response& res){
	int start=0;
	if(req.query().has("start"))
		start=atoi(req.query().get("start").c_str());
	
	res.setHeader("Content-Type","text/json");
	res<<ab->eventQueue.getJSONStringBlock(start);

	return OCS_PROCESSED;
}



/// Lists the XML files at nodes on static subdirs. FIXME use some JSON library.
onion_connection_status NodeManager::list_xml_files(Onion::Request& req, Onion::Response& res)
{
	auto subd=getSubdirectories(AB::static_dir);
	auto I=subd.begin(), endI=subd.end();
	std::string result = "";
	for (;I!=endI;++I){
		std::string dirname=(AB::static_dir + "/" + *I + "/nodes/");
		DIR *dir=opendir(dirname.c_str());
		if (!dir){
			ERROR("Cant read %s directory for xml listings %s", dirname.c_str(), strerror(errno));
			return OCS_INTERNAL_ERROR;
		}
		dirent *ent;
		while ( (ent=readdir(dir)) ){
			//DEBUG("%s", ent->d_name);
			std::string str(ent->d_name);
			if (endswith( str, ".xml")){
				if (result!="")
					result+=" ";
				result+=str;
			}
		}
		closedir(dir);
	}
	Onion::Dict d;
	d.add("files",result);
	return onion_shortcut_response_json(d.c_handler(), req.c_handler(), res.c_handler());
}


onion_connection_status NodeManager::connections(Onion::Request& req, Onion::Response& res)
{
  std::vector<AB::Connection::p> conns;
  if (req.path()!=""){
      AB::Node::p n=ab->getNode(req.path());
      if (!n)
        throw(Onion::HttpInternalError("Cant find node "+req.path()));
      conns=ab->getConnections(n);
    }
  else{
      conns=ab->getConnections();
    }

  std::vector<AB::Connection::p>::iterator I=conns.begin(), endI=conns.end();
  res<<"[";
  for(;I!=endI;){
      AB::Connection::p c=*I;
      res<<"{ \"from\": \""<<c->from()->name()<<"\",";
      res<<" \"to\":\""<<c->to()->name()<<"\",";
      res<<" \"guard\":\""<<c->guard()<<"\" }";
      ++I;
      if (I!=endI)
        res<<",";
    }
  res<<"]";
  return OCS_PROCESSED;
}

onion_connection_status NodeManager::lua(Onion::Request& req, Onion::Response& res){
  Onion::Dict post=req.post();
  if (post.count()){
      std::string luacode=post.get("exec");
      WARNING("%s", luacode.c_str());
      try {
        if (std::find(luacode.begin(), luacode.end(), '=')!=luacode.end()) {
            ab->eval(luacode);
            std::cout<<"--LUA "<<luacode<<std::endl;

          } else {
            ab->eval(std::string("print(")+luacode+")");
          }
      } catch(const std::exception &e) {
        WARNING("%s",e.what());
        std::cout<<"--LUA "<<e.what()<<std::endl;
				ab->eventQueue.pushEvent("lua_exception", "what", e.what() );
      }
      res<<"OK";
      return OCS_PROCESSED;
    }
	res<<"Need exec parameter with lua code to execute at server.";
  return OCS_INTERNAL_ERROR;
}

onion_connection_status NodeManager::uploadXML(Onion::Request &req, Onion::Response &res)
{
	WARNING("Upload XMLs not working yet");
   Onion::Dict post=req.post();
   std::string file="";
   std::string save_ab_file2="";
  if (post.count()){
      save_ab_file=post.get("path");

      file=post.get("files");
      save_ab_file2=save_ab_file+"/"+file;
      ab->clear();
      ab->loadBehaviour(save_ab_file2);
      DEBUG("%s loaded",save_ab_file2.c_str());
      res<<"OK";
      return OCS_PROCESSED;

    }
  return OCS_INTERNAL_ERROR;
}

onion_connection_status NodeManager::uploadWAV(Onion::Request &req, Onion::Response &res)
{
  
  if (req.hasFiles()){

      std::string orig=req.files().get("wavload");
      
      if(!downloaded.empty()) {
          WARNING("%s loaded to %s",orig.c_str(), downloaded.c_str());
          std::string cmd = "cp "+orig+" data/files/audio/"+ downloaded;
          if(system(cmd.c_str()))
            return OCS_INTERNAL_ERROR;
          downloaded.clear();
          res<<"OK";
          return OCS_PROCESSED;
        }
    }
  return OCS_INTERNAL_ERROR;
}

void NodeManager::activateNode(AB::Node::p n)
{
	ab->eventQueue.pushEvent("node_enter_exit", "enter", n->name());
}

void NodeManager::deactivateNode(AB::Node::p n)
{
	ab->eventQueue.pushEvent("node_enter_exit", "exit", n->name());
}

