require(['node','jquery','main'],function(node, $, main){

var CondMerge = function(id, options, behaviour){
	node.Action.call(this, id, options)
	this.x=100
	this.width=2*this.heigth
	this.behaviour=behaviour
	if (!this.param)
		this.param=[]
}
CondMerge.prototype=new node.Action;
CondMerge.prototype.type='condmerge'
CondMerge.prototype.paramOptions=[ ]

CondMerge.prototype.paint = function(opts){
	this.width=this.height
	Action.prototype.paint.call(this, {fill:"#aa00d4"} )
  var g=this.svggroup
	$(g).find('rect#node').attr('transform','matrix(0.70710678,0.70710678,-0.70710678,0.70710678,0,0)').attr('x','5').attr('y','-25')
	$(g).find('text#legend').text('if/endif')
	$(g).find('text#param').remove()
  return g
}


CondMerge.prototype.update = function(){
}

main.behaviour.nodeFactory.add('condmerge',CondMerge)

})
