/**
 * @short Creates a new connection from "from" to "to".
 * 
 * options may be:
 * 
 * color - color of the node
 */
Connection = function(from, to, options){
	if (! (this instanceof Connection)){
		throw new Error("Connection is a class, not a function. Use new.")
	}
	this.id="c_"+Connection.connection_count
	Connection.connection_count+=1
	this.from=from
	this.to=to
	this.guard=''
	// Overwrite attributes with options
	for (var i in options){
		this[i]=options[i]
	}
	if (!this.color)
		this.color='#555'
		
	this.draw()
}

Connection.connection_count=0

Connection.prototype.getSVGPath = function(){
	var frm=this.from
	var to=this.to
	
	var dx=(frm.x+frm.width/2)-(to.x+to.width/2)
	var dy=(frm.y+frm.height/2)-(to.y+to.height/2)
	var adx=Math.abs(dx)
	var ady=Math.abs(dy)
	var ax,ay,bx,by
	
	// I divide the zones in 4 cuadrants, from each corner of the to. n, w, s, e, 0 1 2 3 respectively
	var cuadrant=0

	/// USe this function to check when a point is under a given line, which crosses SW NE, for example.
	var isUnder = function(A,B, P){
		return ( ((B[0]-A[0])*(P[1]-A[1])) - ((B[1]-A[1])*(P[0]-A[0])) ) > 0
	}
	
	var P=[ frm.x+frm.width/2, frm.y+frm.height/2 ]
	var NW = [ to.x, to.y ]
	var NE = [ to.x+to.width, to.y ]
	var SE = [ to.x+to.width, to.y+to.height ]
	var SW = [ to.x, to.y+to.height ]
	
	var uSWNE = isUnder(SW,NE, P)
	var uNWSE = isUnder(NW,SE, P)

	//console.log("At "+uSWNE+" "+uNWSE)
	
	// The cuadrnt is set because of which lines is the point under
	if (uSWNE)
		if (uNWSE)
			cuadrant=2
		else
			cuadrant=1
	else
		if (uNWSE)
			cuadrant=3
		else
			cuadrant=0
	
	/// The extra movement is to make the arrow move on the surface of the to. The formula inside the atan is to take into account that that box has a size.
	switch(cuadrant){
		case 0:
			ax=frm.x+(frm.width/2)*(1+(Math.atan(-dx/200)/1.6))
			ay=frm.y+frm.height
			bx=to.x+(to.width/2)*(1+(Math.atan(dx/200)/1.6))
			by=to.y
			break;
		case 1:
			ax=frm.x
			ay=frm.y+(frm.height/2)*(1+(Math.atan(-dy/200)/1.6))
			bx=to.x+to.width
			by=to.y+(to.height/2)*(1+(Math.atan(dy/200)/1.6))
			break;
		case 2:
			ax=frm.x+(frm.width/2)*(1+(Math.atan(-dx/200)/1.6))
			ay=frm.y
			bx=to.x+(to.width/2)*(1+(Math.atan(dx/200)/1.6))
			by=to.y+to.height
			break;
		case 3:
			ax=frm.x+frm.width
			ay=frm.y+(frm.height/2)*(1+(Math.atan(-dy/200)/1.6))
			bx=to.x
			by=to.y+(to.height/2)*(1+(Math.atan(dy/200)/1.6))
			break;
	}
	var path=svg.createPath()

	if (cuadrant==0 || cuadrant==2){
		var my=(ay+by)/2
		return path.move(ax,ay).curveC(ax,my, bx,my, bx,by);
	}
	else{
		var mx=(ax+bx)/2
		return path.move(ax,ay).curveC(mx,ay, mx,by, bx,by);
	}
}

Connection.prototype.draw=function(){
	var path=this.getSVGPath()
	var color=this.color
	var edges=$('g#edges')

	path=svg.path(edges, path, 
					{width:5, 'marker-end':'url(#Arrow1Lend)', stroke:color, 'stroke-width':2, id:this.id, fill:"none" }
					)
	path=$(path).attr('cursor','pointer')
	path.attr('id','connection_1')
	var t=this
	path.bind('mouseenter',function(){ t.mouseenter() })
	path.bind('mouseout',function(){ t.mouseout() })
	path.bind('click',function(){ t.click() })
	this.svgpath=path
	
	var mx=(this.from.x + this.to.x) / 2
	var my=(this.from.y + this.to.y) / 2
	
	this.svgtext=$(svg.text(edges, mx-(this.guard.length*4.0) ,my, this.guard, {fill:color,stroke:'none'}))
	this.svgtext.bind('mouseenter',function(){ t.mouseenter() })
	this.svgtext.bind('mouseout',function(){ t.mouseout() })
	this.svgtext.bind('click',function(){ t.click() })
	this.svgtext.attr('cursor','pointer')
}

Connection.prototype.update = function(opts){
	if (opts){
		if (opts.to)
			this.to=opts.to
		if (opts.from)
			this.from=opts.from	
		if (opts.color){
			this.color=opts.color
			this.svgpath.attr('stroke',this.color)
		}
	}
	
	var path=this.getSVGPath()
	this.svgpath.attr('d',path._path)
	
	if (this.svgtext){
		var mx=(this.from.x + this.to.x) / 2
		var my=(this.from.y + this.to.y) / 2
		this.svgtext.attr('x',mx).attr('y',my).text(this.guard)
	}
}

Connection.prototype.mouseenter=function(){ 
	this.svgpath.attr('stroke','#aabbcc')
	this.svgtext.attr('fill','#aabbcc')
}

Connection.prototype.mouseout=function(){ 
	this.svgpath.attr('stroke',this.color)
	this.svgtext.attr('fill',this.color)
}

Connection.prototype.click=function(){
	var t=this
	main.showDialog({title:'Conexión', onClose:function(){
		t.setGuard($('#dialog #content input').val())
		
		$.post('/node/'+t.from.id, {guard:t.guard, to:t.to.id}, function(guard){
		  t.setGuard(guard)
		},'text').error(function(){
		  alert(current_language.node_connection_at_server_error)
		  t.setGuard('')
		})

		main.hideDialog()
	}})
	
	var div=$('<div>').append(current_language.guard_msg+'<br>')
	var inpt=$('<input type="text" style="width: 100%">').val(this.guard)
	div.append(inpt)
	$('#dialog #content').html(div)
	inpt.keypress(main.dialogNextOrClose).focus()
}

Connection.prototype.remove = function(){
	this.svgpath.remove()
	this.svgtext.remove()
}

Connection.prototype.setGuard = function(guard){
	this.guard=guard
	this.update()
}
