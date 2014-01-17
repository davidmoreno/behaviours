require(['node','jquery','main'],function(node, $, main){
  
  var Stop=node.extend(node.Action, {})
  
  Stop.prototype.update=function(){
	this.width=this.height
	this.text=''
	$('#'+this.id+' text#param').remove()
	$('#'+this.id+' rect').attr('width',this.width)
  }
  Stop.prototype.activate=function(){
    var text=this.svggroup.find('text')
    text.attr('fill','#00ff00')
    if ($('#startstop.stop').length)
      main.startStop();
  }
  
  main.behaviour.nodeFactory.add('stop',Stop)
})
