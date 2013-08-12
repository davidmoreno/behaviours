/**
 * @short This is the standard implementation for all nodes.
 * 
 * They may reimplement this methods.
 * 
 * this.paint           -- Custom drawing. May call the default one, and work over it. It returns the svg group. Called once at begining.
 * this.update          -- Update the representation of the node on the canvas, as params have changed. Called when framework thinks data need refresh.
 * this.configure       -- Open the configuration dialog.
 * this.acceptConfigure -- Accept the configuration dialog. Must call getParams.
 * this.getParams       -- Return a dict of the params values, updating them from the configure dialog.
 * this.realtime_update -- Call from configure if want to update in realtime status at server; sends the params to the given object, and a exec command on default impl.
 * this.activate        -- This node has been activated at the server (is executing)
 * this.deactivate      -- This node has been deactivated  (is not executing anymore).
 * 
 * Important data fields:
 * 
 * this.paramOptions    -- list of dicts with each a parameter, each parameter with: name,type and text. 
 *                         Depending on type, and if using generic condifure, may have max, min. @see configure.
 * this.params          -- Current dict of params, as in getParams, but also available out of configure, and static out of configure.
 * this.t               -- Type.
 * 
 * More available methods, but more rare to reimplement them:
 * 
 * this.setName         -- Change current name, do change all references through all the graph as needed.
 * this.remove          -- Callback just before node is removed.
 */
Node = function(id, options, behaviour){
	if (! (this instanceof Node)){
		throw new Error("Node is a class, not a function. Use new.")
	}
	var maxY = function(){
		return 10;
	}
	
	if (behaviour){
		this.behaviour=behaviour
		var state=this.behaviour.state
		// Auxiliary functions
		var maxY = function(){
			var mxY=10
			for (var i in state){
				if (state[i].y+state[i].height+20 > mxY)
					mxY=state[i].y+state[i].height+20
			}
			return mxY
		}
	}

	// Now the attributes
	this.active=false;
	this.id=id
	this.x=100
	this.y=100// this.y=maxY()
	this.height=50
	this.width=50
	this.type=''
	
	for (var i in options){
		this[i]=options[i]
	}
	// Set defaults.
	if (!this.params)
	    this.params={}
	    
	
	for (i in this.paramOptions){
	  
	  var po=this.paramOptions[i]
	  
	  if(this.params[po.name]==null)
	    this.params[po.name]=po.default || ''
	  else if (po.type==Number)
	    this.params[po.name]=Number(this.params[po.name])
	}
    
	
	

}

//Node.prototype.paramOptions=[]
Node.prototype.t=''

/**
	* @short Paints on the svg canvas the node
	*/
Node.prototype.paint = function(options){
	var viewpoint=$('g#viewpoint')
	svggroup=svg.group(viewpoint, this.id, {transform:'translate('+this.x+','+this.y+')', stroke:'#000000'} )
	this.svggroup=$(svggroup)
	if (options && options.fill)
		fill=options.fill
	else{
		if (this.t=='action')
			fill='#00aad4'
		else
			fill='#aad400'
	}
	if (!this.width)
		this.width=50
	if (!this.height)
		this.height=50
	this.fill=fill
	var r=svg.rect(svggroup, 0,0, this.width, this.height, 5, 5, {fill:fill,stroke:'none',filter:"url(#dropshadow)",id:'node'})
	svg.text(svggroup, 10,this.height-8, this.type, {'font-family':'Sans','font-size':8,'cursor':'text','stroke-width':0.1})
	svg.text(svggroup, 40,this.height-10, "", {'font-family':'Sans','font-size':this.height-10,'cursor':'text','id':'param','stroke-width':0.1})
	svg.image(svggroup, 4,4, 32,32,"img/"+this.type+".png")
	return this.svggroup
}

/**
	* @short Update the information on the canvas
	*/
Node.prototype.update = function(){

	var txt=null
	var po=null
	if(this.paramOptions) {
	  po=this.paramOptions[0]	  
	  for (i in this.paramOptions){
	    po=this.paramOptions[i]
	    if(po.show)
	      txt=this.params[po.name]
	  }
	}
	
	if(!txt && po) 
	    txt=this.params[po.name] // Any...the last one...
	
	if (!txt){
		txt=current_language.empty
		$('#'+this.id+' text#param').attr('fill','#aaaaaa')
	}
	else{
		txt=String(txt)
		$('#'+this.id+' text#param').attr('fill','black')
	}
	if (txt.length>7)
		txt=txt.substr(0,7)+'...'
		
	var l=Math.ceil(txt.length/4)
	if (l>2)
		l=2
	this.width=l*105+40

	$('#'+this.id+' text#param').text(txt)
	$('#'+this.id+' rect#node').attr('width',this.width)
}

Node.prototype.realtime_update_base = function(){
	if (this.realtime_update){
		console.log("Realtime update: "+this.getParams())
		this.realtime_update(this.getParams())
	}
}

Node.prototype.configureDialogSetup=function(){
	main.showDialog()
	$('#dialog #content').html('')
	var tt=$('#dialog #title').html(current_language.configuration_of)
	var name=$('<a href="#" id="name">'+this.id+'</span>')
	var that=this
	name.click(function(){
		var name=prompt(current_language.set_node_id, that.id)
		that.setName(name)
	})
	tt.append(name).append('</span> <div class="type">('+this.type+')</div>')
	this.dialogShowDefaultButtons()
}

/**
	* @short Shows the dialog to configure this node type, for that id
	*/
Node.prototype.configure=function(){
	this.configureDialogSetup()
 	var that=this

	if (this.paramOptions && this.paramOptions.length!=0){
		var p=this.paramOptions
		var ul=$('<ul>')
		var firstInput=false
		for(var i in p){
			var inpt
			autonext=true
			var container=null
			if (p[i].type==Array){
				inpt=$('<select>')
				for (var j in p[i].values){
					var opt=$('<option>')
					opt.append(p[i].values[j])
					opt.attr('value',j)
					inpt.append(opt)
				}
				inpt.change(function(){
					that.realtime_update_base()
				})
			}
			else if (p[i].type==Number){
				inpt=$('<div>')
				var slider=$('<div>').slider({value:512, max:1024})
				inpt.append(slider)
				var value_=$('<div>').attr('min',p[i].min).attr('max',p[i].max).attr('id','label'+i)
				var updateSlide = function(event, ui){
					var lbl=$(event.currentTarget).next()
					var val=ui.value
					if (lbl.attr('max') && lbl.attr('min')){
						val=(val/1024.0) * (Number(lbl.attr('max'))-Number(lbl.attr('min'))) + Number(lbl.attr('min'))
					}
					val=Math.round(val*100)/100.0
					lbl.text(val)
					that.realtime_update_base()
				}
				
				slider.bind('slide', updateSlide)
				slider.bind('slidechange', updateSlide)
				
				value_.text(0)
				inpt.append(value_)
				inpt.val = function(n){
					if (n==null){
						n=Number($(this).find('#label'+i).text())
						return n
					}
					var iN=Math.ceil((n-Number(p[i].min))/(p[i].max-p[i].min) *1024)
					//console.log("set slider to "+n+" "+iN)
					$(this).find('div:first').slider('value',iN )
				}
			}
			else if (p[i].type==Text){
				container=$('<div id="inputdiv">').append()
				inpt=$('<textarea rows="20" cols="80">')
				container.append(inpt)
				autonext=false
			}
			else{
				container=$('<div id="inputdiv">').append()
				inpt=$('<input>')
				container.append(inpt)
			}
			inpt.attr('id',i)
			var opt=''
			try{
				inpt.val(this.params[this.paramOptions[i].name])
			}
			catch(e){
				if (p[i].type==Number)
					inpt.val(0)
			}
			if (!firstInput){
				firstInput=inpt
			}
			if (autonext)
				inpt.keypress(main.dialogNextOrClose)
			
			var li=$('<li>')
			ul.append(li)
			li.text(p[i].text)
			if (container)
				li.append(container)
			else
				li.append(inpt)
		}
		var id=this.id
		$('#dialog #content').html(ul)
		firstInput.focus()
	}
	if (this.paramDoc){
		$('#dialog #content').append($('<div class="doc">').html(this.paramDoc))
	}

}

/// Method to be called when the object is activated, this is, its notified to execute.
Node.prototype.activate = function(){
  this.active=true
  var text=this.svggroup.find('text')
  text.attr('fill','#00ff00')
  
}

/// Method to be called when the object is activated, this is, its notified to execute.
Node.prototype.deactivate = function(){
  this.active=false
  var text=this.svggroup.find('text')
  text.attr('fill','#00ff00')
  setTimeout(function(){
    text.attr('fill','green')
    setTimeout(function(){
	text.attr('fill','black')
    },300)
  },300)
}


/**
 * @short Shows the default buttons of the dialog, as when opening a dialog for manipulating nodes
 */
Node.prototype.dialogShowDefaultButtons = function(){
	var node=this
	main.closeDialog = function(){
		if (node){
			node.acceptConfigure()
			node.behaviour.sendNodeParams(node)
		}
		main.closeDialogDefault()
	}
		
  $('#dialog #custom_buttons').html('')
  $('#dialog #custom_buttons').append($('<a href="#">').text(current_language.delete_node).click(function(){ if(node.behaviour.deleteNode(node)) main.hideDialog();  }))
  $('#dialog #custom_buttons').append($('<a href="#">').text(current_language.connect_disconnect).click(function(){ node.behaviour.view.setNodeToConnect(node); main.hideDialog();  }))
}

Node.prototype.getParams = function(){
	var p=this.paramOptions
	if (p){
		var dialog=$('#dialog:visible')
		if (dialog.length==1){
			var params={}
			for(var i in p){
				var val
				if (p[i].type==Number){
					val=Number($('#dialog #content #label'+i).text())
				}
				else{
					val=$('#dialog #content #'+i).val()
				}
				params[p[i].name]=val
			}
		return params;
		}
		return this.params;
	}
	return {}
}

/**
	* @short 
	*/
Node.prototype.acceptConfigure=function(){
	try{
		this.params=this.getParams()
		this.update()
	}
	catch(e){
		alert(current_language.alert_accept_configure);
	}
}

Node.prototype.focus = function(){
	$(this.svggroup).find('#node').attr('fill','#5f5')
}

Node.prototype.blur = function(){
	$(this.svggroup).find('#node').attr('fill',this.fill)
}

Node.prototype.position = function(x,y){
	if (x && y){
		this.svggroup.attr('transform', 'translate('+x+','+y+')');
	}
}

Node.prototype.setName = function(new_name){
	if (new_name == this.id)
		return
	if (new_name in this.behaviour.state){
		alert(current_language.new_name_exists_error)
		return
	}
	var that=this
	this.behaviour.sendNodeName(this, new_name, function(){
		var state=that.behaviour.state
		delete state[that.id]
		$('#'+that.id).attr('id',new_name)
		that.id=new_name
		state[that.id]=that
		$('#dialog #name').text(new_name)
	})
}

Node.prototype.remove = function(){
}

/**
 * @short Some helper functions
 */
NodeHelper={
	updateFromArray : function(that){
	  
		if (!that.params)
			return
			
		var txt=[]
		for (var i in that.paramOptions){
			if (that.paramOptions[i].type==Array)
				txt.push(that.paramOptions[i].values[that.params[that.paramOptions[i].name]])

			else
				txt.push(that.params[that.paramOptions[i].name])
		}
		txt=txt.join(' · ')
		that.width=txt.length*25 + 40
		$('#'+that.id+' text#param').text(txt)
		$('#'+that.id+' rect#node').attr('width',that.width)
	}
}


/**
 * @short Base for all actions
 */
Action = function(id, options, behaviour){
	Node.call(this, id,options, behaviour)
//	this.x=700
}
Action.prototype=new Node;
Action.prototype.t='action'


/**
 * @short Base for all events
 */
Event = function(id, options, behaviour){
	Node.call(this, id, options, behaviour)
	this.t='event'
//	this.x=100
}
Event.prototype=new Node;
Event.prototype.t='event'

Event.prototype.paint = function(options){
	var viewpoint=$('g#viewpoint')
	svggroup=svg.group(viewpoint, this.id, {transform:'translate('+this.x+','+this.y+')', stroke:'#000000'} )
	this.svggroup=$(svggroup)
	
	fill='#aad400'
	if (!this.width)
		this.width=200
	if (!this.height)
		this.height=50
	this.fill=fill
		
	var path=svg.createPath()
	//path=path.move(0,97).curveC(0,this.height-3, -1,this.height-2, 3,this.height).curveC(this.width-3,this.height, this.width-2, this.height, this.width-3, this.height)
	//path=path.curveC(this.width, 3, this.width, 2, this.width,3)
	
	var h=this.height
	var w=this.width
	var h2=h/2
	var r=svg.group(svggroup, {fill:fill,stroke:'none',filter:"url(#dropshadow)"})
	
	path=path.move(-h2,0).line(5,0).line(5,h).line(-h2,h).line(0,h2)
	
	svg.rect(r, 0,0, this.width, this.height, 5, 5,{stroke:'none',id:'node'})
	
	svg.path(r, path,{stroke:'none'})

	svg.text(svggroup, 18,this.height-8, this.type, {'font-family':'Sans','font-size':8,'cursor':'text','stroke-width':0.1,'id':'legend'})
	svg.text(svggroup, 4,this.height-8, "N", {'font-family':'Sans','font-size':8,'cursor':'text','stroke-width':0.1,'id':'legend'})
	svg.text(svggroup, 50,this.height-10, "", {'font-family':'Sans','font-size':this.height-10,'cursor':'text','id':'param','stroke-width':0.1})
	svg.image(svggroup, 14,4, 32,32,"img/"+this.type+".png")
	svg.image(svggroup, 0,1, 16,16,"img/ok.png")

	return this.svggroup
}


/**
 * @short Default methods extend helper
 * 
 * This helper extends a base class with some more prototype attributes. Used to extend 
 * Nodes.
 */
extend = function(base, options, behaviour){
	var that = function(id, options, behaviour){
		base.call(this, id, options, behaviour)
	}
	that.prototype=new base;

	for (var i in options){
		that.prototype[i]=options[i]
	}

	
	return that
}
