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

#ifndef AB_MANAGER
#define AB_MANAGER

#include "object.h"

#include <boost/thread.hpp>

#include <string>
#include <set>
#include <map>
#include <vector>
#include <queue>

namespace AB {
  class Action;
  class Event;
  class Node;
  class Connection;
  class Resolver;
  class LUA;

  /**
   * @short Stores the behaviour metadata: name, description and GUI current viewpoint.
   *
   * Provided in metadata of xml or through GUI
   */
  class Meta {
  public:
    std::string name;
    std::string description;
    std::vector<double> viewpoint;
    
  };
  
  /**
   * @short Manages the behaviour graph
   *
   * It keeps the information about all available nodes and connections, so it does the necesary polling on events,
   * notifies nodes as they must be checked/activated...
   *
   * Normally the use is loading the XML via loadBehaviour, but it may be constructed by hand using the
   * addNode and connect methods.
   *
   * The manager is also in charge of givin information and resolving OCL expressions.
   */
  class Manager {
  public:
    Manager();
    ~Manager();

    /**
     * @short Clears the manager, but keeps the AB::Node::ManualRemoval nodes
     *
     * Also connections are kept when possible.
     */
    void clear();
    /**
     * @short Loads a behaviour from the XML file.
     */
    void loadBehaviour(const std::string &xmlfile);
     /**
     * @short Saves a behaviour to a XML file.
     */
    void saveBehaviour(const std::string &xmlfile, bool includeFiles = false);

    /**
     * @short Adds a node to the manager.
     */
    void addNode(Node *);
    /**
     * @short Deletes a node from the manager. If also_delete_object is true, it is deleted. 
     */
    void deleteNode(Node *n, bool also_delete_object=true);

    /**
     * @short Notifies that this event is activated, so connected nodes must be notified as appropiate.
     *
     * When a node is notified, the connected nodes guards are checked. If any is true, that connection is performed.
     * If no guard conforms, and there are connections without guards they are randomly applied.
     *
     * Notify will execute a simple node connection, the first that applies.
     *
     * If current thread is the same as the exec thread, then its fired inmediatly, else its enqueued and will be
     * launched at the exec loop. This allows to crete new threads to check conditions, and those will send the events as
     * necesary, not from the main loop, and then properly enqueued.
     */
    void notify(Node *);
    /**
     * @short Connects two nodes
     */
    Connection *connect(Node *, Node *);
    /**
     * @short Connects two nodes by id.
     */
    Connection *connect(const std::string idA, const std::string idB);

		/**
		 * @short Disconnects two nodes
		 */
		void disconnect(Node *A, Node *B);
		
		/**
		 * @short Disconnects all from a node
		 */
		void disconnect(Node *A);
		
		
		/**
		 * @short Returns the connection, if any between two nodes.
		 * 
		 * @return The connection, or NULL if none.
		 */
		Connection *getConnection(Node *A, Node *B);

		/**
		 * @short Returns all connection objects
		 */
		std::vector<Connection *> getConnections();

		/**
		 * @short Returns all connection objects from a given node
		 */
		const std::vector<Connection *> &getConnections(Node *A);

    /**
     * @short Returns the list of nodes
     */
    const std::set<Node*> &getNodes();
    /**
     * @short Returns the list of active events, this is events that can happend in any moment.
     */
    const std::set<Event*> &getActiveEvents();

    /**
     * @short Gets a node by name
     */
    Node *getNode(const std::string &id);
    /**
     * @short Gets an event by name
     */
    Event *getEvent(const std::string &id);
    /**
     * @short Gets an action by name
     */
    Action *getAction(const std::string &id);

    /**
     * @short Checks a LUA expression.
     */
    bool check(const std::string &expr, const std::string &name="=inline_code");
    /**
     * @short Evals a LUA expression.
     */
    Object eval(const std::string &expr, const std::string &name="=inline_code");

    /**
     * @short Resolves a node from the expression
     *
     * It returns the node that matches the node name or type, or a value in last or any node.
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
    AB::Object resolve(const std::string &name_or_type_or_just_value);

    /**
     * @short Parses a text to change the {{ }} tags with the LUA code it executes.
     *
     * This way its easy to integrate elements that depend on a fixed string, but you want that string to
     * change at runtime.
     *
     * Each node may or may not use this feature, but if used must follow this standard. For example:
     *
     * "Hello {{user}}".
     */
    std::string parseString(const std::string &);

    /**
     * @short Starts the execution of the manager. It will run until cancel() is called.
     */
    void exec();
    /**
     * @short Stops the execution of the manager.
     */
    void cancel() { running_=false; }
    
     /**
     * @short Returns true if the manager is currently running, false otherwise
     */
    bool isRunning() { return running_; }

    /**
     * @short Returns the LUA object, so that we can register/manipualte from elsewhere.
     *
     * Normally it is to register some extra functions at init, like, for example, AISoy1 registers the bot object.
     */
    LUA *getLUA() { return lua; }
    
     /**
     * @short Sets the user given behaviour name.
     *
     * Provided in metadata of xml or through GUI
     */
    void setName(const std::string & n_) { metadata.name = n_; }
     /**
     * @short Returns the user given behaviour name.
     *
     * Provided in metadata of xml or through GUI
     */
    const std::string &name() const { return metadata.name; }

     /**
     * @short Sets the user given behaviour description.
     *
     * Provided in metadata of xml or through GUI
     */
    void setDescription(const std::string & d_) { metadata.description = d_; }
     /**
     * @short Returns the user given behaviour description.
     *
     * Provided in metadata of xml or through GUI
     */
    const std::string &description() const { return metadata.description; }
    
     /**
     * @short Returns the GUI canvas viewpoint.
     *
     * Provided in metadata of xml or through GUI. Only useful within GUI.
     */
    void setViewpoint(const std::vector<double> & v_) { metadata.viewpoint = v_; }
    void setViewpoint(const std::string & str);
     /**
     * @short Returns the GUI canvas viewpoint.
     *
     * Provided in metadata of xml or through GUI. Only useful within GUI.
     */
    const std::vector<double> &viewpoint() const { return metadata.viewpoint; }
    
  private:
    void sync();
    Node *notifyOne(Node *);

  private:
    LUA *lua;
    std::set<Event *> activeEvents;
    std::set<Node*> nodes;
    Node *lastNode;
    std::queue<Node *> pendingNotifications;
    boost::mutex pendingNotificationsMutex;
    Meta metadata;
    

    std::map<Node*, std::vector<Connection*> > nodeConnections;

    volatile bool running_;
    bool syncOnNextCycle; ///< Whenever some part of the graph changes, new event, new action, new connection.. or removal, next cycle must sync.

    boost::thread::id execThreadId;
  };

  /// Overwrite to get a notification on each node notify enter
  extern void (*manager_notify_node_enter)(AB::Node *node);
  /// Overwrite to get a notification on each node notify exit
  extern void (*manager_notify_node_exit)(AB::Node *node);
}

#endif
