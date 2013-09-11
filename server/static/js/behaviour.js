
define(['jquery','node_factory','connections'],function($, node_factory,Connection){
var Behaviour = function(view){
	if ( ! (this instanceof Behaviour) ){
		throw("Behaviour is a class. Create it with new.")
	}
	
	this.state={}
	this.connections={}
	this.node_count=0
	// Flag to know if there is a radio manager already
	this.hasRadio=false
	// Interlink view/controller and model.
	this.view=view
	view.behaviour=this 
	// Metadata
	this.name=""
	this.description=""
	this.ready=false;
//	this.activeNodes=null;
	this.nodeFactory=new node_factory.NodeFactory(this)
	
}

Behaviour.prototype.addNode = function(type, id, params, position, only_client) {
	var that = this
	if (type==null)
	  throw("Need a type")
	  
	if (!this.ready)
	  throw("System busy")
	
	if(!only_client) {
	  // In  case the user tries to create a RadioReceive Event without a RadioManager,
	  // alert the user and create a RadioManager
	  if ( !this.hasRadio && (type=="radio_receive" || type=="radio_send") ) {
	    alert(current_language.radioerror)
	    this.addNode('radio_manager','radio_manager',null)
	    return
	  }
	  // In case the user tries to create a RadioManager when there already exists one, 
	  // alert the user and don't do it
	  if ( this.hasRadio && type=="radio_manager" ) {
	    alert(current_language.radioredundant)
	    return
	  }
	}    
	this.node_count+=1  
	
	if (id==null){
	  id=type+'_'+this.node_count
	}
	if (type=='radio_manager') {
	  this.hasRadio=true 
	}

	var node=new (this.nodeFactory.get(type))(id, {type:type, params:params}, this)
	
	if(position) {
	  node.x = position.x
	  node.y = position.y
	}
	
	if(only_client){
	  
	  this.state[id]=node
	  
	  var create_params=jQuery.extend({}, node.getParams())
	  create_params.create_node=type;
	  
	  that.view.addNode(node)
	  
	  
	} else {
	  
	  if ($('#startstop.stop').length)
	    main.startStop(true);
      
	  this.state[id]=node
	  var create_params=jQuery.extend({}, node.getParams())
	  create_params.create_node=type;
	  this.ready = false
	  $('#loading').show()
	  $.post('/node/',create_params, function(name){
		  // I got a name, and confirmation of creation.
		  delete that.state[node.id]
		  node.id=name
		  that.state[name] = node

		  if (!params && node.paramOptions){
			  node.configure()
			  $('#dialog #content').attr('nodeId', node.id)
		  }
		  that.view.addNode(node)
		  if(!position)
		    that.view.resetViewpoint() 
		  $.post('/node/'+node.id,{x:node.x, y:node.y}, function(){
		    $.post("/manager/",{save:0}, function(){
		      $('#loading').hide()
		      that.ready = true
		    }) 
		  }).error(function(txt){
		  alert(current_language.parameter_setting_error+txt.responseText)
		  })
	  },'text').error(function(){
		  alert(current_language.node_creation_at_server_error)
		  that.deleteNode(node, true)
	  })
	}
	
	return node
}

Behaviour.prototype.deleteNode = function(node, no_confirm, only_client){
  	if (!this.ready || node.active)
	  throw("System busy")
  
	if(!only_client) {
	  if (node.id!='radio_manager') {
	    if (!no_confirm && !confirm(current_language.deleteconfirm))
	      return false
	  } else {
	    // In case the user tries to delete the RadioManager, 
	    // alert the user that all RadioSend and RadioReceive nodes will be deleted as well
	    if (!confirm(current_language.deleteradio)) {
	      return false
	    } else {
	      // In case the user accepts, 
	      // update corresponding flag and delete all RadioReceive and RadioSend nodes
	      this.hasRadio=false 
	      for (var i in this.state){
		if(this.state[i].type.match("radio_send")=="radio_send" || this.state[i].type.match("radio_receive")=="radio_receive") 
		  this.deleteNode(this.state[i],true);
	      }
	    }
	  }
	}
	// remove connections
	this.disconnect(node, null, only_client)
	var that=this
	
	// Now really remove if order didn't come from server
	if(!only_client) {
	  
	    if ($('#startstop.stop').length)
	      main.startStop(true);
      
	    that.ready=false
	    $('#loading').show()
	    $.post('/node/'+node.id,{remove:true}, function(){
	      delete that.state[node.id]
	      that.view.deleteNode(node)
	      $.post("/manager/",{save:0}, function(){
		$('#loading').hide()
		that.ready = true
	      })
	    }).error(function(){
	      alert(current_language.node_removal_at_server_error);
	    })
	} else {
	  delete that.state[node.id]
	  that.view.deleteNode(node)
	}
	return true
}

Behaviour.prototype.get = function(id){
	return this.state[id]
}

Behaviour.prototype.connect = function(from, to, color, id){
	if (!this.ready)
	  throw("System busy")
	  
	var that=this
	var c=new Connection(from, to, {color:color})
	if(!id) {
	  
	  if ($('#startstop.stop').length)
	    main.startStop(true);
      
	  this.ready=false
	  $('#loading').show()
	  $.post('/node/'+c.from.id, {connect_to:to.id}, function(name){
	    c.id = name
	    that.ready=true
	    $('#loading').hide()
	  },'text').error(function(){
		  alert(current_language.node_connection_at_server_error)
		  that.disconnect(c)
	  })
	} else {
	  c.id = id;
	}
	this.connections[c.id]=c
	return c
}

Behaviour.prototype.disconnect = function(from, to, only_client){
  
	if (!to){
		// Removing single connection.
		if (from instanceof Connection){
			  
			var c=from
			var that=this
			console.log("Removing connection "+c.id)
			if(!only_client) {
			  $('#loading').show()
			  $.post('/node/'+c.from.id, {disconnect_from:c.to.id},function(){
				  // Really removes when server acknowledges.
				  c.remove()
				  for (var i in that.connections){
					  if (that.connections[i]==c)
						  delete that.connections[i]
				  }
				  $('#loading').hide()
			  }).error(function(){
				  alert(current_language.node_connection_at_server_error)
				  that.disconnect(c)
			  })
			} else {
			  
			  c.remove()
			  for (var i in that.connections){
			    if (that.connections[i]==c)
			      delete that.connections[i]
			  }
			}
			return true;
		}
		
		// Removing all connection from/to a given node
		var ret=false
		var id=from.id
		for (var i in this.state){
			for (var c in this.connections){
				if (!(this.connections[c].to_be_removed) && (this.connections[c].from.id==id || this.connections[c].to.id==id)){
					ret=this.disconnect(this.connections[c],null,only_client)
					if(this.connections[c])
					  this.connections[c].to_be_removed=true // Mark it as its removal has been programmed.
				}
			}
		}
		return ret
		
	} else {
		// Remove specific connection
		var connections=this.connections
		var c=this.getConnection(from, to)
		if (c){
			return this.disconnect(c,null,only_client)
		}
		return false
	}
}

Behaviour.prototype.getConnection = function(from, to, excluded){
	var connections=this.connections
	for (var i in connections){
		if (connections[i].from==from && connections[i].to==to && excluded!=connections[i]){
			return connections[i]
		}
	}
	return null
}

Behaviour.prototype.sendNodeParams = function(node){
	var params=node.params
	for (var k in params){
		$.post("/node/"+node.id,{attr:k,value:params[k]}).error(function(txt){
			alert(current_language.parameter_setting_error+txt.responseText)
		})
	}
}

Behaviour.prototype.sendNodeName = function(node, new_name, fn){
	$.post("/node/"+node.id,{name:new_name}, fn).error(function(txt){
		alert(current_language.node_name_change_error+txt.responseText)
	})
}


Behaviour.prototype.clear = function(only_client){
  
  for (var i in this.state){
    this.deleteNode(this.state[i], true, true)
  }

  this.node_count=0
  // Flag to know if there is a radio manager already
  this.hasRadio=false
  // Metadata
  this.name=""
  this.description=""
  
  this.view.resetViewpoint()
  
  if(!only_client) {
    $.post("/manager/",{clear:true}).error(function(txt){
	  alert(current_language.behaviour_clearing_at_server_error+txt.responseText)
    })
  }
}

Behaviour.prototype.setMetaData = function(onclose){
  if( !$('#overwindow').is(':visible') ) {
    
    $('#overwindow').fadeIn('fast')
    $('#dialog #buttons').hide()
    $('#dialog #title').text(current_language.properties)
    
    var ul = $('<ul>')
    var li = $('<li>')
    
    onEnter='key=window.event.keyCode; if(key==13) main.closeDialog();'
    
    var inpt=$('<input id="metaname" onkeydown="'+onEnter+'">')
    li.text(current_language.appname+'  ')
    li.append(inpt)
    ul.append(li)
  
    li = $('<li>')
    inpt=$('<textarea id="metadesc" rows="10" cols="40" onkeydown="'+onEnter+'">')
    li.text(current_language.appdescription)
    li.append('<br><br>')
    li.append(inpt)
    ul.append('<br>')
    ul.append(li)

    $('#dialog #content').html(ul)
    
    var n = document.getElementById('metaname')
    
    var d = document.getElementById('metadesc')
    
    n.value = this.name
    d.value = this.description
    var that = this
    main.closeDialog = function() {
      var n = document.getElementById('metaname')
      var d = document.getElementById('metadesc')
      that.name = n.value;
      that.description = d.value;
      $('#overwindow').fadeOut('fast')

      $.post("/manager/",{name:that.name,description:that.description}).error(function(txt){
	  alert(current_language.parameter_setting_error+txt.responseText)
	  $.post("/manager/",{save:0},function(){
			if (onclose)
				onclose()
		})
	})
    }
    
    
  } else {
  
    main.closeDialog()
  }
  
  
}
	return { Behaviour:Behaviour }
	
})
