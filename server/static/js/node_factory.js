define(['jquery','node'],function($,node){

var NodeFactory=function(behaviour){
	if (! (this instanceof NodeFactory)){
		throw new Error("NodeFactory is a class, not a function. Use new.")
	}
	this.known_types={}
	this.javascripts=[]
	this.behaviour=behaviour
	this.updateAvailableNodes()
}

NodeFactory.prototype.updateAvailableNodes = function(){
	var that=this
	$.get('/node/?list_types', function(d){
		$.get('/node/?list_files', function(filelist){ 
		  var files = filelist.files.split(" ");
			var count=files.length
		  for(var n in files) {
		    $.get('nodes/'+files[n], function(xml){ 
		      that.parseNodeDescription(xml); 
					count--;
					if (count==0){
						console.log('Read extra js: '+that.javascripts)
						require(['main'].concat(that.javascripts), function(main){
							main.ready()
						})
					}
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
		if (!icon)
			icon=id+'.png'
		icon='img/'+icon

		var klass
		var li=$('<li>').attr('node-type',id)
		var a=$('<a href="#">').attr('node-type',id).attr('title',this.getTranslatedField(xml, 'description'))
		var d=$('<div class="buttoncover">')
		var img=$('<img src="'+icon+'">')
		var br=$('<br>')
	
		li.draggable({helper:'clone',stack:'.svgscroll', opacity:0.5})
		li.bind('dragstop',function(event, ui){
		
		// Transform from cursor coordinates to svg coordinates
		  var pc={left:event.originalEvent.pageX, top:event.originalEvent.pageY}
		  var canvas=$('svg').position()
		  var p = {x:pc.left-canvas.left, y:pc.top-canvas.top}
		  
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
		})
		a.append(d).append(img).append(br).append(name)
		li.append(a)
		a.click(function(){ that.behaviour.addNode($(this).attr('node-type')) })
		if (type=="action"){
			$('#actionlist').append(li)
			klass=node.Action
		}
		else{
			$('#eventlist').append(li)
			klass=node.Event
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

		this.known_types[id]=node.extend(klass, {paramOptions:paramOptions})
		
		// Add update from array function in case one of the paramOptions is an Array
		if(hasArray) {
		  this.known_types[id].prototype.update = function() {
		    node.NodeHelper.updateFromArray(this);
		  }
		  
		}
		xml.children('js').each(function(){ 
			that.javascripts.push($(this).text().trim().slice(0,a.length-4))
		})
	}
}

NodeFactory.prototype.get = function(type){
	return this.known_types[type]
}

NodeFactory.prototype.add = function(name, type){
	this.known_types[name]=type
}

return {NodeFactory:NodeFactory}
})
