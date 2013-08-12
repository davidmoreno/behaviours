function NodeFactory(behaviour){
	if (! (this instanceof NodeFactory)){
		throw new Error("NodeFactory is a class, not a function. Use new.")
	}
	this.known_types={}
	this.behaviour=behaviour
	this.updateAvailableNodes()
	
	
}

NodeFactory.prototype.updateAvailableNodes = function(){
	var that=this
	$.get('/node/?list_types', function(d){
		$.get('/node/?list_files', function(filelist){ 
		  var files = filelist.files.split(" ");
		  for(var n in files) {
		  	var para=files[n]
		    $.get('nodes/'+files[n], function(xml){ 
		      that.parseNodeDescription(xml); 
		    }, 'xml')
		  }
		  that.behaviour.ready=true;
		  // mark as behaviour ready to start working
		  
		},'json')
	}, 'json')
}

NodeFactory.prototype.getTranslatedField = function(xml, tag){
	var r=xml.children(tag+'[lang='+current_language.lang+']')
	if (r.length)
		return r.text()
	else
		return xml.children(tag).text()
}

NodeFactory.prototype.paramTypes={ string:String, float:Number, array:Array, text:Text }

NodeFactory.prototype.parseNodeDescription = function(xml){

	var that=this
	var x=$(xml).children('node-descriptions').children('node-description')
	if (!x.length)
		x=$(xml).children('node-description')
	for(i=0;i<x.length;i++){
		var xml=$(x[i])
		var id=xml.attr('id')
		// alert(id);
		if (id in this.known_types)
			continue;
		var name=this.getTranslatedField(xml, 'name')

		var type=xml.children('type').text()
		var icon=xml.children('icon').attr('src')
		var pluginname= xml.children('pluginname').text();

		//Crear barra del plugin si no está creada
		if(pluginname ){
			
			if($('#classes ul').find('#'+pluginname+'b').length==0){
				var lis=pluginname+"list"
				var idname= pluginname+"b"
				$('#classes ul').append('<li><a href="#" onclick="main.canvas.changeTool(\''+pluginname+'\')" id='+idname+' lid='+idname+'>'+pluginname+'</a></li>')
				$('#extension').append('<div id ='+pluginname+' style="display: none"> <ul id='+lis+' class="toolbuttons"></ul></div>')
				
			}
			
		}
		
		if (!icon)
			icon=id+'.png'
		icon='img/'+icon

		var klass
		var li
		if (type=="action"){			
				li=$('<li class="action">').attr('node-type',id)
		}
		else {			
				li=$('<li class="event">').attr('node-type',id)
		}
		
		var a=$('<a href="#">').attr('node-type',id).attr('title',this.getTranslatedField(xml, 'description'))
		var d=$('<div class="buttoncover">')
		var img=$('<img src="'+icon+'">')
		var br=$('<br>')
	
		
		li.draggable({helper:'clone',stack:'.svgscroll', opacity:0.5,drag:function(e){
			var toolsPosition = $('#tools').position()
			var dragging = $('.ui-draggable-dragging')
			if(dragging.position().top+dragging.height() >= toolsPosition.top){
				dragging.css('background','red');
			}else{
				dragging.css('background','');
			}
		}})
		li.bind('dragstop',function(event, ui){
		
		// Transform from cursor coordinates to svg coordinates
		  var pc={left:event.originalEvent.pageX, top:event.originalEvent.pageY}
		  var canvas=$('svg').position()
		  var p = {x:pc.left-canvas.left, y:pc.top-canvas.top}
		  
		  var toolsPosition = $('#tools').position()
		  var dragging = $('.ui-draggable-dragging')
	      if(dragging.position().top+dragging.height() < toolsPosition.top){

			  var pos = that.behaviour.view.root.createSVGPoint();
			  pos.x = p.x
			  pos.y = p.y
			  var gr
			  if(!that.behaviour.view.svgRoot)
			    gr = that.behaviour.view.getRoot(that.behaviour.view.root);
			  else
			    gr = that.behaviour.view.svgRoot

			  pos = pos.matrixTransform(gr.getCTM().inverse());
	    
	      
			  that.behaviour.addNode($(this).attr('node-type'),null,null,{x:pos.x,y:pos.y})
		  }
		})
		a.append(d).append(img).append(br).append(name)
		li.append(a)
		a.click(function(){ that.behaviour.addNode($(this).attr('node-type')) })

		if(pluginname){

			$('#'+pluginname+"list").append(li)
			if (type=="action"){			
				klass=Action
			}
			else {			
				klass=Event
			}

		}
		else if (type=="action"){
			$('#actionlist').append(li)
			klass=Action
		}
		else {
			$('#eventlist').append(li)
			klass=Event
		}

		
		var hasArray=false;
		var paramOptions=[]
		$.each(xml.children('params').children('param'),function(){
			var p=$(this)
			var type=that.paramTypes[p.children('type').text().toLowerCase()]
			var desc=that.getTranslatedField(p,"description")
			var pdict={type:type, text:desc, name:p.attr('id') }
			if (type==Number){
				pdict['min']=Number(p.children('min').text())
				pdict['max']=Number(p.children('max').text())
			} else if (type==Array) {
			  hasArray=true;
			  pdict['values']=that.getTranslatedField(p,"values").split(",")
			}
			var _def=p.children('default').text()
			if (_def) {
			  if (type==Number || type ==Array){
				pdict['default']=Number(_def)
			  } else {
				pdict['default']=_def
			  }
			}
			paramOptions.push( pdict )	
			
		})

		this.known_types[id]=extend(klass, {paramOptions:paramOptions})
		
		// Add update from array function in case one of the paramOptions is an Array
		if(hasArray) {
		  this.known_types[id].prototype.update = function() {
		    NodeHelper.updateFromArray(this);
		  }
		  
		}
		// Last thing, load JS that can overwrite it all. Load them in order. Slower, but safer.
		{
			var load_in_order = function(jss){
				if (jss.length==0)
					return
				var js=jss[0]
				jss=jss.slice(1)
				$.getScript('js/'+js, function(){ load_in_order(jss) }) 
			}
			var jss=[]
			xml.children('js').each(function(){ 
				jss.push($(this).text())
			})
			
			load_in_order(jss)
		}
	}
}

NodeFactory.prototype.get = function(type){
	return this.known_types[type]
}

NodeFactory.prototype.add = function(name, type){
	this.known_types[name]=type
}
