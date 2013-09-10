require(['node','jquery','main'],function(node, $, main){
/**
 * @short Click is an event, triggered when the node is clicked by the user
 */

  var Click = node.extend(node.Event, {})


  
  Click.prototype.paint = function(options){
    this.width=150
    this.height=50
    Event.prototype.paint.call(this, options)	
    $('#'+this.id+' text#param').text("Click")
    return this.svggroup
  }

  /**
  * @short On click, show configuration dialog or trigger event, depending on program status (running/stopped)
  */
  Click.prototype.configure=function(){
    if ($('#startstop.play').length) {
      Node.prototype.configure.call(this)
    } else {
      main.lua_exec('manager.notify('+this.id+')')
    }
  }

  /**
  * @short Don't update the information on the canvas (no parameters)
  */
  Click.prototype.update = function(){

  }

  main.behaviour.nodeFactory.add('click',Click)
})
