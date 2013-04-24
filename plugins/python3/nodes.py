"""
Node interface for Behaviours.

Usage:

	from ab.nodes import nodes

	print(nodes.timeout.count)
	nodes.timeout.count=10

	print(dir(nodes))
	print(dir(nodes.timeout))
	
Right now you have to embed manually this code on every behaviour that needs nodes. Will be fixed ASAP.
"""

import ab
class Nodes(object):
  def __getattr__(self, name):
   return ab.resolve(name)
  def __getitem__(self, name):
   return ab.resolve(name)
  def __dir__(self):
   d=object.__dir__(self)
   return d+ab.list_nodes()

nodes=Nodes()
