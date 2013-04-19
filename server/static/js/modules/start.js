(function(){
var Start=extend(Event, {})
Start.prototype.update=function(){
	this.width=this.height
	this.text=''
	$('#'+this.id+' text#param').remove()
	$('#'+this.id+' rect').attr('width',this.width)
}
main.behaviour.nodeFactory.add('start',Start)
}())
