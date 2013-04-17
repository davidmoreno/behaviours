NOTES ABOUT REST INTERFACE
==========================


When not specified, returns "OK" on success and 505 and description of error on failure.

Following calls are available:

/node/
------

RETURNS:
  JSON object with nodes; node id as key, attributes as values.


Create Node
'''''''''''

POST: 
  create_node=[type]
RETURNS: 
  node_name
  
List node types
'''''''''''''''

GET: 
  list_types
RETURNS: 
  JSON list of known node types

/node/[node-name]
-----------------

Connect nodes
'''''''''''''

POST: 
  connect_to=[other_node_name]
OPTIONAL:
  guard=[Guard text]
  
Disconnect nodes
''''''''''''''''

POST: 
  disconnect_from=[other_node_name]

Set attributes
''''''''''''''

POST:
  attr=attribute key name
  value=new value
  
Notify a node
'''''''''''''

This activates the server side of the node, starting the chain of actions.

POST:
  notify

  
/connection/
------------

RETURNS:
  JSON list of connections: for each connection it has from, to and guard.
  
/connection/[node_name]
'''''''''''''''''''''''

RETURNS:
  JSON list of connections to this node: for each connection it has from, to and guard.
