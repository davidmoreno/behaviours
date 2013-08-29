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

#include <sstream>

#include "log.h"
#include "manager.h"
#include "lua.h"
#include "event.h"
#include "action.h"
#include "connection.h"
#include "pluginloader.h"

using namespace AB;

void readBehaviour(const std::string &file, Manager *manager);
void writeBehaviour(const std::string &file, Manager *manager, bool includeFiles = false);

Manager::Manager() :
  lua(NULL),
  lastNode(NULL),
  running_(false),
  syncOnNextCycle(true)
{
  srand(time(NULL));
  lua=new LUA();
  lua->setManager(this);
  execThreadId=std::this_thread::get_id(); // Can be used directly, if before calling exec.
  PluginLoader::loadPath(AB_PREFIX "/lib/ab/");
}

Manager::~Manager()
{
  std::map<Node*,std::vector<Connection*> > oldConnections=nodeConnections;
  nodeConnections.clear();
  typedef std::pair<Node*,std::vector<Connection*> > pair;
  for(pair p: oldConnections) {
    if (nodes.count(p.first)>0) {
      for(Connection *s: p.second) {
        delete s;
      }
    }
  }
  for(Node *ev: nodes) {
    delete ev;
  }
  if (lua)
    delete lua;
}

void Manager::clear()
{
  DEBUG("Clear");

  std::set<Node*> keepNodes;
  activeEvents.clear();

  for(Node *n: nodes) {
    if (n->flags()&Node::ManualRemoval) {
      keepNodes.insert(n);
      Event *ev=dynamic_cast<Event*>(n);
      if (ev) {
        activeEvents.insert(ev);
      }
    } else {
      DEBUG("Delete %p", n);
      delete n;
    }
  }

  nodes=keepNodes;

  std::map<Node*,std::vector<Connection*> > oldConnections=nodeConnections;
  nodeConnections.clear();
  typedef std::pair<Node*,std::vector<Connection*> > pair;
  for(pair p: oldConnections) {
    if (nodes.count(p.first)>0) {
      for(Connection *s: p.second) {
        if (nodes.count(s->to())>0) {
          Connection *nc=connect(p.first, s->to());
          nc->setGuard(s->guard());
        }
        delete s;
      }
    }
  }
#ifdef __DEBUG__
  for(pair p: nodeConnections) {
    for(Connection *s: p.second) {
      DEBUG("%s;",s->unicode().c_str());
    }
  }
#endif

  syncOnNextCycle=true;
}

void Manager::loadBehaviour(const std::string &xmlfile)
{
  DEBUG("Reading behaviour from %s", xmlfile.c_str());
  readBehaviour(xmlfile, this);
}

void Manager::saveBehaviour(const std::string &xmlfile, bool includeFiles)
{
  DEBUG("Saving behaviour %s", xmlfile.c_str());
  writeBehaviour(xmlfile, this, includeFiles);
}

void Manager::addNode(Node *n)
{
  DEBUG("Add node %s %p", n->name().c_str(), n);
  // If node id already exists in behaviour, find a non-existing one
  while(getNode(n->name()))
    n->setName();
  n->setManager(this);
  Event *ev=dynamic_cast<Event*>(n);
  if (ev) {  
    activeEvents.insert(ev);
    printf("%s\n","activeEvents lenght" );
    printf("%d\n", activeEvents.size());
    DEBUG("%s at active events", ev->name().c_str());
  }
  nodes.insert(n);
   printf("%s\n","nodes lenght" );
  printf("%d\n", nodes.size());
  syncOnNextCycle=true;
}
void Manager::removeEvent( std::string id){
    for(Event *ev: activeEvents) {
      if(strcmp(ev->name().c_str(),id.c_str())==0){
        WARNING("SON IGUALES");
        activeEvents.erase(ev);
      }

    }

}
void Manager::addEvent(Event *ev){
  if(ev){
    printf("%s\n","activeEvents lenght" );
    printf("%d\n", activeEvents.size());
    activeEvents.insert(ev);

    printf("%d\n", activeEvents.size());
  }
}
bool Manager::findNode(std::string id){
   for(Event *ev: activeEvents) {
      if(strcmp(ev->name().c_str(),id.c_str())==0){
        return true;
      }

    }
    return false;
}

void Manager::deleteNode(Node *n, bool also_delete_object){
	DEBUG("Delete node %s %p", n->name().c_str(), n);
	
	disconnect(n);
	
  Event *ev=dynamic_cast<Event*>(n);
  if (ev) {
		activeEvents.erase(ev);
	}
	nodes.erase(n);
	if (also_delete_object)
		delete n;
}

Connection *Manager::connect(Node *A, Node *B)
{
  /* if(A->name().compare(B->name())==0){
     DEBUG("No connect same node %s -> %s", A->name().c_str(), B->name().c_str());
    return NULL;
  }*/
  Connection *conn=getConnection(A,B);
  if (conn) // If exists, return existing one.
    return conn; 
 conn=new Connection(this, A, B);
  WARNING("si que llega por aqui");
  if(getEvent(B->name())!=NULL){
    Object newob= to_object(1);
    B->setAttr("nodeon",newob);
  }
  nodeConnections[A].push_back(conn);
  syncOnNextCycle=true;
  return conn;
}


Connection *Manager::connect(const std::string idA, const std::string idB)
{
  DEBUG("Connect %s -> %s", idA.c_str(), idB.c_str());
  Node *A=getNode(idA);

  if (!A)
    return NULL;

  Node *B=getNode(idB);

  if (!B)
    return NULL;


  return connect(A, B);
}

void Manager::disconnect(Node *A, Node *B){
	std::vector<Connection*> &conns=nodeConnections[A];
	std::vector<Connection*>::iterator I=conns.begin(), endI=conns.end();
  Event *ev=dynamic_cast<Event*>(B);
  if(ev){
        printf("%s\n","A ver como queda desde nodo" );
        printf("%d\n", ev->nodeon); 
        Object newob= to_object(0);
        ev->setAttr("nodeon",newob);
  }

	for(;I!=endI;++I){
		if ((*I)->to()==B){
			Connection *c=*I;
			conns.erase(I);
			delete c;
			return;
		}
	}
}

void Manager::disconnect(Node *A){
	{ // Easy part, from A
		std::vector<Connection*> &conns=nodeConnections[A];
		std::vector<Connection*>::iterator I=conns.begin(), endI=conns.end();
		for(;I!=endI;++I){
			delete *I;
		}
		conns.clear();
	}
	
	{ // A bit more convoluted, to A
		std::map<Node*, std::vector<Connection*> >::iterator I=nodeConnections.begin(), endI=nodeConnections.end();
		for(;I!=endI;++I){
			std::vector<Connection*> &conns=I->second;
			std::vector<Connection*>::iterator J=conns.begin(), endJ=conns.end();
			do{
				for(;J!=endJ;++J){
					if ((*J)->to()==A){
						Connection *c=*J;
						conns.erase(J);
						delete c;
						break;
					}
				}
			}while(J!=endJ); // Stopped before end, maybe one removed, J is invalidated.
		}
	}
}


Connection *Manager::getConnection(Node *A, Node *B){
	std::vector<Connection*> &conns=nodeConnections[A];
	std::vector<Connection*>::iterator I=conns.begin(), endI=conns.end();
	for(;I!=endI;++I){
		if ((*I)->to()==B)
			return *I;
	}
	return NULL;
}

std::vector< Connection* > Manager::getConnections()
{
	std::vector< Connection* > ret;
	std::map<Node*, std::vector<Connection*> >::iterator I=nodeConnections.begin(), endI=nodeConnections.end();
	for(;I!=endI;++I){
		ret.insert(ret.end(), I->second.begin(), I->second.end());
	}
	return ret;
}

const std::vector<Connection *> &Manager::getConnections(Node *A)
{
	return nodeConnections[A];
}

void Manager::sync()
{
#ifdef __DEBUG__
  typedef std::pair<Node*, std::vector<Connection*> > pair;

  for(pair c: nodeConnections) {
    for(Connection *conn: c.second) {
      DEBUG("%s", conn->unicode().c_str());
    }
  }
#endif

  DEBUG("Sync!");
  for(Event *ev: activeEvents) {
    DEBUG("Check %s: %X, %s", ev->name().c_str(), ev->flags(), ev->flags()&Event::NeedSync ? "true" : "false");
    if (ev->flags()&Event::NeedSync) {
      ev->sync();
    }
  }
  syncOnNextCycle=false;
}


void Manager::exec()
{
  int alwaysExec=11;
  DEBUG("Start execution of behaviour");
  execThreadId=std::this_thread::get_id();
  int t=0;
  running_=true;
	syncOnNextCycle=true;
  while(running_) {
    if (syncOnNextCycle)
      sync();
    //DEBUG("Checks at t=%d", t);
    for(Event *ev: activeEvents) {
      //DEBUG("Check %s %d", ev->name().c_str(), ev->flags());
      if (ev->flags()&Event::Polling) {
        DEBUG("Cont: %d and noderepeat: %d",ev->cont,ev->noderepeat);
        if(ev->noderepeat!=alwaysExec && ev->cont ==ev->noderepeat){
          DEBUG("Event %s is removed!", ev->name().c_str());
            ev->cont=0;
            Object newob= to_object(1);
            ev->setAttr("nodeon",newob);
        }
        if (ev->check()) {
          DEBUG("Event %s is triggered!", ev->name().c_str());
          WARNING("ioahsfioashfuiasfhauisfhasufhasdfasdfasdfasdfasdf");
          notify(ev);
        }
        if (syncOnNextCycle) // It will continue with the list, unless a sync (graph change) is performed.
          break;
      }
    }
    {
      std::lock_guard<std::mutex> l(pendingNotificationsMutex);
      while (!pendingNotifications.empty()) {
        Node *n=pendingNotifications.front();
        pendingNotifications.pop();
        if (nodes.count(n)>0)
          notify(n);
      }
    }
    usleep(200000);
    t++;
  }
  DEBUG("Finished execution of behaviour");
}

/**
 * The pendingNotifications queue is checked for integrity before launching the new node notification to check its still
 * valid.
 */
void Manager::notify(Node *node)
{
  if (std::this_thread::get_id()!=execThreadId) {
    DEBUG("Pushing for notification at proper thread");
    std::lock_guard<std::mutex> l(pendingNotificationsMutex);
    pendingNotifications.push(node);
  } else {
    try {
      lastNode=node;
      while(lastNode) {
        lastNode=notifyOne(lastNode);
        if(lastNode){
            Event *ec=dynamic_cast<Event*>(lastNode);
            if (ec) {
              WARNING("El evento es: %s",ec->name().c_str());
              addEvent(ec);
              lastNode=NULL;
            }
        }
      }
    } catch(const std::exception &e) {
      ERROR("Catched unhandled exception: %s! Stoping this chain.", e.what());
    } catch(...) {
      ERROR("Catched unhandled exception! Stoping this chain.");
    }
  }
}

/// Callback to know when a node is notified on enter.
void (*AB::manager_notify_node_enter)(AB::Node *node) = NULL;
/// Callback to know when a node is notified on exit
void (*AB::manager_notify_node_exit)(AB::Node *node) = NULL;

/// Fast and dirty RAII to enforce call of exit when node exits.
class RAII_enter_exit_node {
  typedef void (*fn)(AB::Node *);
  fn exit;
  Node *data;
public:
  RAII_enter_exit_node(fn enter, fn _exit, Node *_data) : exit(_exit), data(_data) {
    if (enter)
      enter(data);
  }
  ~RAII_enter_exit_node() {
    if (exit)
      exit(data);
  }
};

/**
 * @short Executes the action if any, and checks which one is next to execute, if any, returns it
 *
 * Using this helper funcion its possible to avoid recursion.
 *
 * @param node The node to exec.
 * @returns The next node to execute.
 */
Node *Manager::notifyOne(Node *node)
{
  RAII_enter_exit_node een(manager_notify_node_enter, manager_notify_node_exit, node);
  Action *ac=dynamic_cast<Action*>(node);
  if (ac) {
    ac->exec();
  }
  if (nodeConnections.count(node)>0) {
    DEBUG("Notify node %s", node->name().c_str());
    std::vector<Connection*> p=nodeConnections[node];
    for(Connection *conn: p) {
      if (conn->checkGuard()) {
        return conn->to();
      }
    }
    // Ok, do one random without the guards. Nor truly normal distribution, but should be ok for small <1000 numbers.
    int n=0;
    for(Connection *conn: p) {
      if (conn->guard()=="") {
        n++;
      }
    }
    if (n>0) {
      n=rand()%n;
      for(Connection *conn: p) {
        if (conn->guard()=="") {
          if (n==0)
            return conn->to();
          n--;
        }
      }
    }
  } else {
    DEBUG("End of execution chain at %s",node->name().c_str());
  }
  return NULL;
}


const std::set<Node*> &Manager::getNodes()
{
  return nodes;
}

const std::set<Event*> &Manager::getActiveEvents()
{
  return activeEvents;
}

Node *Manager::getNode(const std::string &id)
{
  for(Node *ev: nodes) {
    if (ev->name()==id)
      return ev;
  }
  return NULL;
}

Event *Manager::getEvent(const std::string &id)
{
  Node *n=getNode(id);
  if (n)
    return dynamic_cast<Event*>(n);
  return NULL;
}

Action *Manager::getAction(const std::string &id)
{
  Node *n=getNode(id);
  if (n)
    return dynamic_cast<Action*>(n);
  return NULL;
}


bool Manager::check(const std::string &ex, const std::string &name)
{
  return lua->check(ex,name);
}

Object Manager::eval(const std::string &ex, const std::string &name)
{
  return lua->eval(ex,name);
}

/**
 * @short Returns the object.
 *
 * Resolves the object using these order:
 *
 * 1. Global object names
 * 2. All objects of the given type. If last node is that type, then for sure last node.
 * 3. Attributes at last node
 * 4. Attributes at any node, unknown order.
 *
 * Resolving can become ambigous, but only if accessing not through object name. All other accesses are
 * auxiliary to help with development, but not recomended.
 */
AB::Object Manager::resolve(const std::string &var)
{
  DEBUG("Resolving variable %s", var.c_str());
  // Check it its an object name
  for(Node *nn: nodes) {
    if (nn->name()==var) {
      DEBUG("Found as name");
      return to_object(nn);
    }
  }

  // Check if its type of object.
  if (lastNode && lastNode->type()==var) {
    return to_object(lastNode);
  }

  for(Node *nn: nodes) {
    if (nn->type()==var) {
      DEBUG("Found as type: %s", nn->type());
      return to_object(nn);
    }
  }
  // Now last chance, ignore obj as not found, ask attr to all. First lastNode.
  if (lastNode) {
    try {
      return lastNode->attr(var);
    } catch(const AB::attribute_not_found &e) {
    }
  }

  // Ok, just look at attrs at any known object
  for(Node *nn: nodes) {
    try {
      return nn->attr(var);
    } catch(const AB::attribute_not_found &e) {
      /// Ignore not found.
    }
  }

  DEBUG("There is no such variable");
  throw(AB::attribute_not_found(var));
}

std::string Manager::parseString(const std::string &str)
{
  // opos, open position; cpos close position.
  size_t opos=str.find("{{");
  if (opos==str.npos)
    return str;
  std::stringstream ret;
  size_t cpos=0;
  while (opos!=str.npos) {
    ret<<str.substr(cpos, opos-cpos);
    cpos=str.find("}}",opos);
    if (cpos==str.npos) { // Bad formed, nothing passes.
      ret<<str.substr(opos);
      return ret.str();
    }
    cpos+=2;
    ret<<object2string(eval("return "+str.substr(opos+2, cpos-opos-4)));

    opos=str.find("{{", cpos);
  }
  ret<<str.substr(cpos);
  return ret.str();
}

void Manager::setViewpoint(const std::string &str) {
  std::string matrix = str;

  std::vector<double> v;
  size_t next_pos;
  std::string element;
  if( (next_pos = matrix.find('(')) != std::string::npos)
    matrix = matrix.substr(next_pos+1);

  while (next_pos != std::string::npos) {
    next_pos = matrix.find(',');
    element = matrix.substr(0,next_pos);
    v.push_back(atof(element.c_str()));
    matrix = matrix.substr(next_pos+1);
  }
  if (v.size() != 6) {
    WARNING("Wrong viewpoint matrix (size = %d)", (int)v.size());
    return;
  } else { 
    DEBUG("Setting viewpoint: (%f,%f,%f,%f,%f,%f)", v[0],v[1],v[2],v[3],v[4],v[5]);
    setViewpoint(v);
  }
}