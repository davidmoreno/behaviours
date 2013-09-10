define(['jquery','extra/jquery.svg'],function($){

var toolbutton_width = 90;

var Canvas = function(id, options){
	if (! (this instanceof Canvas)){
		throw new Error("Canvas is a class, not a function. Use new.")
	}
	this.oldConnectLine=null
	this.nodeToConnect=null
	this.svg=null
	this.edges=null
	this.viewpoint=null
	this.selected=null
	this.connecting_dialog=false
	this.behaviour=null
	
	// Tools visibility state variables 
	this.toolShowedName='none'
	this.toolShowedIndex=0
	
	// Pan and Zoom state variables
	this.zoomScale = 0.4; // Zoom sensitivity
	this.root;
	this.st = 'none';
	this.svgRoot = null;
	this.stTarget;
	this.stOrigin;
	this.stTf;
	this.ready=false;
}

Canvas.prototype.setNodeToConnect = function(node){
	this.nodeToConnect=node;
	main.showMessage(current_language.connect_message)
	node.focus()
}

Canvas.prototype.addNode = function(node){
	var canvas=this
	var that=this
	var g=node.paint()
	node.update()

	g.attr('cursor','pointer')
	g.find('text').attr('cursor','pointer')
	g.draggable()
	g.click( function(){
		if (that.nodeToConnect){
			if (canvas.behaviour.getConnection(that.nodeToConnect, node))
				canvas.behaviour.disconnect(that.nodeToConnect, node)
			else
				canvas.behaviour.connect(that.nodeToConnect, node)
			main.hideMessage()
			that.nodeToConnect.blur()
			that.nodeToConnect=null;
			return;
		}
		node.configure(); 
		$('#dialog #content').attr('nodeId', node.id) 
	} )
	
	var getCursorAtCanvas = function(that, event){
		var pos={left:event.originalEvent.pageX, top:event.originalEvent.pageY}
		var canvas=$('svg').position()
		return {x:pos.left-canvas.left, y:pos.top-canvas.top}
		//return {x:event.offsetX || event.layerX || event.originalEvent.clientX, y:event.offsetY || event.layerY || event.originalEvent.clientY}
	}
	
	g.bind('dragstart',function(event, ui){
		if (event.ctrlKey){
			$('g').attr('cursor','e-resize')
			$('text').attr('cursor','e-resize')
			node.drag=true
			if (canvas.oldConnectLine)
				canvas.oldConnectLine.remove()
			var pos=getCursorAtCanvas(this,event)
			var to={x:pos.x, y:pos.y, width:1, height:1, id:'tmp'}	
			canvas.oldConnectLine=new Connection(node, to, '#55a')
		}
		else{
			node.drag=false
			$(event.target).attr('cursor','move')
			$(event.target).find('text').attr('cursor','move')
		}
		
	})
	
	g.bind('drag',function(event, ui){
		var p=getCursorAtCanvas(this, event)
		
		// Transform from cursor coordinates to svg coordinates
		var pos = canvas.root.createSVGPoint();
		pos.x = p.x
		pos.y = p.y
		var gr
		if(!canvas.svgRoot)
		  gr = canvas.getRoot(canvas.root);
		else
		  gr = canvas.svgRoot

		pos = pos.matrixTransform(gr.getCTM().inverse());
	    
		var x=pos.x
		var y=pos.y
		if (node.drag){
			canvas.selected=null
			$('g').attr('stroke', 'black')
			
			var state=that.behaviour.state
			var id=node.id
			for (var i in state){
				if (i!=id){
					if (x>state[i].x && x<state[i].x+state[i].width &&
							y>state[i].y && y<state[i].y+state[i].height){
						var c=canvas.behaviour.getConnection(state[id], state[i], canvas.oldConnectLine)
						if (c)
							canvas.oldConnectLine.update({to:state[i], color:'#d55'})
						else
							canvas.oldConnectLine.update({to:state[i], color:'#5d5'})
						canvas.selected=i
					}
				}
			}
			if (!canvas.selected){
				var to={x:x, y:y, width:1, height:1, id:'tmp'}
				canvas.oldConnectLine.update({to:to, color:'#55a'})
			}
		}
		else{
			//x-=node.width/2
			//y-=node.height/2
			//$('#msg').text("Position "+x+","+y)
			node.position(x,y)
			node.x=x
			node.y=y
			canvas.updateEdges(node)
		}
	})
	
	g.bind('dragstop',function(event, ui){ 
		if (canvas.oldConnectLine){
			canvas.oldConnectLine.remove()
			canvas.oldConnectLine=null
		}
		var state=that.behaviour.state
		if (node.drag && canvas.selected){
			var c=canvas.behaviour.getConnection(node, state[canvas.selected], canvas.oldConnectLine)
			if (!c){
				canvas.behaviour.connect(node, state[canvas.selected])
			}
			else{
				canvas.behaviour.disconnect(node, state[canvas.selected])
			}
			canvas.updateEdges(node)
		}
		node.drag=false
		g.attr('stroke', 'black').attr('cursor','pointer')
		g.find('text').attr('cursor','pointer')
		$.post("/node/"+node.id,{x:node.x, y:node.y}).error(function(txt){
		alert(current_language.parameter_setting_error+txt.responseText)
		})
	})
	
	node.update()
	return g 
}


Canvas.prototype.deleteNode = function(node){
	$('g#viewpoint #'+node.id).remove()
}

Canvas.prototype.setupGraph = function(svg_, that){
  
	svg=svg_
	
	var shadow='<filter id="dropshadow" height="130%">'+
		'<feGaussianBlur in="SourceAlpha" stdDeviation="2"/> <!-- stdDeviation is how much to blur -->'+
		'<feOffset dx="2" dy="2" result="offsetblur"/> <!-- how much to offset -->'+
		'<feMerge> '+
		' <feMergeNode/> <!-- this contains the offset blurred image -->'+
		' <feMergeNode in="SourceGraphic"/> <!-- this contains the element that the filter is applied to -->'+
		'</feMerge>'+
		'</filter>'

	svg.load('<svg version="1.1" xmlns:svg="http://www.w3.org/2000/svg"  xmlns="http://www.w3.org/2000/svg">'+
			'<defs>  <marker orient="auto" refY="0" refX="0" id="Arrow1Lend" style="overflow:visible">'+
			'<path d="M -5,-5 L 0,0 L -5,5 L 0,0  z " style="fill:none;stroke:#555;stroke-width:1pt;marker-start:none" transform=""/>'+
			'</marker>'+ shadow +
			'</defs>'+
			'</svg>')
/*
	for (var x=0;x<3000;x+=230){
		for (var y=0;y<5000;y+=230){
			svg.image(x,y, 230,230, "static/img/wallpaper.jpg")
		}
	}
	*/
	//edges=svg.group('edges')
	   
	if(that)
	  that.setupViewpoint(that)
	else
	  this.setupViewpoint()
	  
}

/**
 * @short Resets viewpoint values
 */
Canvas.prototype.setupViewpoint = function(source){
  
  var that
  if(source)
    that=source
  else
    that = this
  
  that.svgRoot = null;
  that.viewpoint=svg.group('viewpoint')
  //var r=svg.circle(viewpoint, 100,100, 30,{fill:'#333',stroke:'none',id:'origin'})
  var o=svg.image(viewpoint, 50,50, 90,90,"img/origin.png")
  that.edges=svg.group(that.viewpoint,'edges')	
  that.root = $('#svgcanvas').svg('get').root();  	
  that.setupHandlers(that);
  that.getRoot(that);
  // mark canvas as ready to work with it
  that.ready = true;
}

/**
 * @short Adds mouse events to be handled by svg canvas
 */
Canvas.prototype.setupHandlers = function(source){
  
	var that
	if(source) 
	  that=source
	else
	  that = this
	  
	if(navigator.userAgent.toLowerCase().indexOf('webkit') >= 0)
		window.addEventListener('mousewheel', function(evt){that.handleMouseWheel(evt,that);}, false); // Chrome/Safari
	else
		window.addEventListener('DOMMouseScroll', function(evt){that.handleMouseWheel(evt,that);}, false); // Others
	
	
	
	window.addEventListener('mouseup', function(evt){that.handleMouseUp(evt,that);}, false);
	window.addEventListener('mousedown', function(evt){that.handleMouseDown(evt,that);}, false);
	window.addEventListener('mousemove', function(evt){that.handleMouseMove(evt,that);}, false);
	window.addEventListener('mouseout', function(evt){that.handleMouseUp(evt,that);}, false);
	window.addEventListener('dblclick', function(evt){that.handleDoubleClick(evt,that);}, false);
	
}

Canvas.prototype.getRoot = function (source){
  
  	var that
	if(source)
	  that=source
	else
	  that = this
	  
	if(that.svgRoot == null) {
		var r = that.root.getElementById("viewpoint") ? that.root.getElementById("viewpoint") : that.root.documentElement, t = r;

		while(t != that.root) {
			if(t.getAttribute("viewBox")) {
				that.setCTM(r, t.getCTM());

				t.removeAttribute("viewBox");
			}

			t = t.parentNode;
		}

		that.svgRoot = r;
	}
	
	return that.svgRoot;
}

/**
 * @short Instance an SVGPoint object with given event coordinates.
 */
Canvas.prototype.getEventPoint = function (evt) {
	var p = this.root.createSVGPoint();

	p.x = evt.clientX;
	p.y = evt.clientY;

	return p;
}

/**
 * @short Sets the current transform matrix of an element.
 */
Canvas.prototype.setCTM = function (element, matrix) {
	var s = "matrix(" + matrix.a + "," + matrix.b + "," + matrix.c + "," + matrix.d + "," + matrix.e + "," + matrix.f + ")";
	element.setAttribute("transform", s);
}


/**
 * @short Sends latest viewpoint matrix to server
 */
Canvas.prototype.updateServer = function (current) {
  
  var s
  var matrix = current
  
  if(!matrix)
    matrix = this.stTf;
  
  if(!matrix)
    s = "matrix(1,0,0,1,0,0)"
  else
    s = "matrix(" + matrix.a + "," + matrix.b + "," + matrix.c + "," + matrix.d + "," + matrix.e + "," + matrix.f + ")";
  
  $.post("/manager/",{viewpoint:s}).error(function(txt){
    alert(current_language.parameter_setting_error+txt.responseText)
  })
}


/**
 * @short Handle mouse move event.
 */
Canvas.prototype.handleMouseMove = function (evt,source) {
  
  
  var that
  if(source)
    that=source
  else
    that = this
  
  if(evt.target.tagName!='svg')
    return
  if(evt.preventDefault)
	  evt.preventDefault();

  evt.returnValue = false;

  var g
  if(!that.svgRoot)
    g = that.getRoot(that.root);
  else
    g = that.svgRoot
  
  if(that.st == 'pan') {
    // Pan mode
    var p = that.getEventPoint(evt).matrixTransform(that.stTf);
    that.setCTM(g, that.stTf.inverse().translate(p.x - that.stOrigin.x, p.y - that.stOrigin.y));

  }
}

/**
 * @short Handle click event.
 */
Canvas.prototype.handleMouseDown = function (evt,source) {

	var that
	if(source)
	  that=source
	else
	  that = this
	  
	if(evt.target.tagName!='svg')
	  return
	  
	if(evt.preventDefault)
		evt.preventDefault();

	evt.returnValue = false;

	  var g
	  if(!that.svgRoot)
	    g = that.getRoot(that.root);
	  else
	    g = that.svgRoot
	
	// Pan mode
	that.st = 'pan';
	that.stTf = g.getCTM().inverse();
	that.stOrigin = that.getEventPoint(evt).matrixTransform(that.stTf);

}

/**
 * @short Handle mouse button release event.
 */
Canvas.prototype.handleMouseUp = function (evt,source) {
      
	var that
	if(source)
	  that=source
	else
	  that = this
	  
	if(evt.target.tagName!='svg')
	  return
	
	if(evt.preventDefault)
		evt.preventDefault();

	evt.returnValue = false;

	if(that.st == 'pan') {
	  var g
	  if(!that.svgRoot)
	    g = that.getRoot(that.root);
	  else
	    g = that.svgRoot
	  
	  that.stTf = g.getCTM().inverse();
	  // Quit pan/scroll mode
	  that.st = '';
	  
	  this.updateServer(g.getCTM().inverse())
	}
}

/**
 * @short Handle mouse wheel event.
 */
Canvas.prototype.handleMouseWheel = function (evt,source) {

	var that
	if(source){
	  that=source
	  
	} else {
	  that = this
	}
	var delta;

	if(evt.wheelDelta)
		delta = evt.wheelDelta / 360; // Chrome/Safari
	else
		delta = evt.detail / -9; // Mozilla
		
		
	// If on top of '#tools' or its children, let list of actions/events scroll
	if(that.toolShowedName != 'none' && (evt.target.tagName == 'DIV' || evt.target.tagName == 'UL' || evt.target.tagName == 'IMG')) {
	  if(delta < 0)
	    that.toolsScrollR()
	  else
	    that.toolsScrollL()
	    
	  return  
	}
	
	if(evt.target.tagName!='svg')
		return
    
	if(evt.preventDefault)
		evt.preventDefault();

	evt.returnValue = false;

	var z = Math.pow(1 + that.zoomScale, delta);
	
	var g
	if(!that.svgRoot)
	  g = that.getRoot(that.root);
	else
	  g = that.svgRoot

	var p = that.getEventPoint(evt);
	p = p.matrixTransform(g.getCTM().inverse());

	// Compute new scale matrix in current mouse position
	var k = that.root.createSVGMatrix().translate(p.x, p.y).scale(z).translate(-p.x, -p.y);

        that.setCTM(g, g.getCTM().multiply(k));


	that.stTf = g.getCTM().inverse();

	
	this.updateServer(g.getCTM().inverse())
}


/**
 * @short Reset Pan and Zoom when doube clicked.
 */
Canvas.prototype.handleDoubleClick = function (evt,source) {
    
  var that
  if(source)
    that=source
  else
    that = this
	  
  if(evt.target.tagName!='svg')
    return
  if(evt.preventDefault)
	  evt.preventDefault();

  evt.returnValue = false;
  
  that.resetViewpoint()
}

/**
 * @short Show all nodes in canvas.
 */
Canvas.prototype.viewAll = function () {

  var g

  if(!this.svgRoot)
   g = this.getRoot(this.root);
  else
    g = this.svgRoot
  
  this.setCTM(g, g.getCTM().inverse().multiply(g.getCTM()));
  this.stTf = g.getCTM().inverse();
  
  var max_x, max_y, min_y, min_x
  
  var state = this.behaviour.state
  for (var i in state){
    
    if(!max_x || !max_y || !min_x || !min_y) {
      max_y = state[i].y + state[i].height
      max_x = state[i].x + state[i].width
      min_y = state[i].y
      min_x = state[i].x
      
    } else {
      if( (state[i].y + state[i].height) > max_y)
	max_y = state[i].y + state[i].height
      if((state[i].x + state[i].width)> max_x)
	max_x = state[i].x + state[i].width
      if(state[i].y < min_y)
	min_y = state[i].y
      if(state[i].x < min_x)
	min_x = state[i].x
    }
  }
  var p = this.root.createSVGPoint();
  var canvas_center = this.root.createSVGPoint();
  
  var z = 0;
  
  p.x = Math.round((min_x+max_x)/2);
  p.y = Math.round((min_y+max_y)/2);
   
  var w=$(window).width()
  var h=$(window).height()-$('#header').height()-$('#tools').height()
  
  canvas_center.x = Math.round(w/2)
  canvas_center.y = Math.round(h/2)
    
  xscale = (w-50)/Math.abs(max_x-min_x)
  yscale = (h-30)/Math.abs(max_y-min_y)
  
  if(yscale < xscale)
     z = yscale
   else z = xscale

  this.setCTM(g, this.stTf.inverse().translate(canvas_center.x-p.x, canvas_center.y-p.y));
  this.stTf = g.getCTM().inverse();
  
  // Compute new scale matrix in current mouse position
  var k = this.root.createSVGMatrix().translate(p.x, p.y).scale(z).translate(-p.x, -p.y);
	
  this.setCTM(g, g.getCTM().multiply(k)); 

  this.stTf = this.stTf.multiply(k.inverse());
  
  this.updateServer(g.getCTM().inverse())
	
}

/**
 * @short Reset Pan and Zoom.
 */
Canvas.prototype.resetViewpoint = function() {
  
  var g
  
  if(!this.svgRoot)
   g = this.getRoot(this.root);
  else
    g = this.svgRoot
  
  this.setCTM(g, g.getCTM().inverse().multiply(g.getCTM()));
  this.stTf = g.getCTM().inverse();

  this.updateServer(g.getCTM().inverse())
}

/**
 * @short Sets a given viewpoint (s is a string: matrix(a,b,c,d,e,f)
 */
Canvas.prototype.setViewpoint = function(s) {
  
  if(this.svgRoot == null)
    return

  var g = this.svgRoot

  g.setAttribute("transform", s)
  this.stTf = g.getCTM()
  this.setCTM(g, g.getCTM().inverse())

}

Canvas.prototype.updateEdges = function(node){
	var connections=node.behaviour.connections
	for (var cid in connections){
		connections[cid].update()
	}
}

Canvas.prototype.updateCanvasSize = function(source){
      
      var that
      if (source)
	that = source
      else
	that = this
	
      var svgs=$('#svgscroll')
      var w=$(window).width()
      var h=$(window).height()-$('#header').height()-$('#tools').height()
      svgs.css('width',''+(w-2)+'px').css('height',''+h+'px')
      $('#tools').width(w)
	
      if(that.toolShowedName == 'none') {
	
	$('#help').attr('style','bottom: 28px;')
	$('#lua_console').attr('style','bottom: 28px;')
	$('#console').attr('style','bottom: 28px;')
	$('#webdav').attr('style','bottom: 28px;')
    
      } else {
	
    	$('#help').attr('style','bottom: 148px;')
	$('#lua_console').attr('style','bottom: 148px;')
	$('#console').attr('style','bottom: 148px;')
	$('#webdav').attr('style','bottom: 148px;')
      }
      
      $('#help').height($(window).height()-$('#header').height()-$('#tools').height())
      $('#console').height($(window).height()-$('#header').height()-$('#tools').height())
      $('#lua_console').height($(window).height()-$('#header').height()-$('#tools').height())
      $('#webdav').height($(window).height()-$('#header').height()-$('#tools').height())
      
      that.updateTools()

}

Canvas.prototype.changeTool = function(tool){
  this.toolShowedIndex = 0
  var btn=$('#'+tool+'b')
  var btnOld=$('#'+this.toolShowedName+'b')
  if (this.toolShowedName != tool) {
    $('#tools #'+this.toolShowedName).hide()
    this.toolShowedName = tool
    btnOld.removeClass('selected')
    btn.addClass('selected')
    $('#tools #extension').show()
    $('#tools #'+this.toolShowedName).show()
    $('#tools').attr('style','height: 150px;')
    
    $('#help').attr('style','bottom: 148px;')
    $('#lua_console').attr('style','bottom: 148px;')
    $('#console').attr('style','bottom: 148px;')
    $('#webdav').attr('style','bottom: 148px;')
    
  } else {
    this.toolShowedName = 'none'
    $('#tools #extension').hide()
    $('#tools #'+tool).hide()
    btn.addClass('selected') 
    $('#tools').attr('style','height: 30px;')

  }

  this.updateCanvasSize(this)
  
}

Canvas.prototype.updateTools = function(){
  
	if(this.toolShowedName == 'none')
	  return
	  
	var li_length = $('#tools #'+this.toolShowedName+' ul li').length
	
	var w = $('#tools').width()
	
	if (li_length*toolbutton_width+20 > w) {
	  if( this.toolShowedIndex > 0) {
	    $('#tools #scroll_left').show();
	  } else {
	    $('#tools #scroll_left').hide();
	  }
	  if( this.toolShowedIndex < li_length-Math.floor(w/toolbutton_width)) {
	    $('#tools #scroll_right').show();
	  } else {
	    $('#tools #scroll_right').hide();
	  }
	  
	} else {
	  $('#tools #scroll_left').hide();
	  $('#tools #scroll_right').hide();
	}
	
	$('#tools #'+this.toolShowedName+' li:hidden').show();	
	$('#tools #'+this.toolShowedName+' li:lt('+this.toolShowedIndex+')').hide();
	
}

Canvas.prototype.toolsScrollL = function(){
  if( !$('#tools #scroll_left').is(':visible') )
    return;
  
  var w = $('#tools').width()  
  this.toolShowedIndex = this.toolShowedIndex-Math.floor(w/toolbutton_width)+1
  if (this.toolShowedIndex < 0)
    this.toolShowedIndex = 0;
  this.updateTools()
}

Canvas.prototype.toolsScrollR = function(){
  if( !$('#tools #scroll_right').is(':visible') )
    return;
  var w = $('#tools').width()
  this.toolShowedIndex = this.toolShowedIndex+Math.floor(w/toolbutton_width)-1
  this.updateTools()
}

	return {Canvas:Canvas}
})
