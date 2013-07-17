/**
 */

(function()
{
var showInterface = function(){
  // Esto es como llamar a this.configureDialogSetup pero sin showDefaultButtons, con lo que no asigna el acceptConfig al close
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

  var tr=$('<tr>')
  var table=$('<table>').append(tr)

  

    
//   if (this.params.exec)
//     code.val(this.params.exec)
//   if (this.params.check)
//     code.val(this.params.check)
    
  var cheat_sheet=$('<ul>')
	var state=main.behaviour.state
  var b= $('<div class="buttonsPanel">').css({'margin-left': 'auto','margin-right': 'auto','width':'200px'})

    b.append("<button onclick='clickButton(this.id)' style='margin-left:30px' id= 'Up'>Up</button>").append("<br/>").append("<button onclick='clickButton(this.id)' id='Left'>Left</button>").append("<button onclick='clickButton(this.id)' id='Right'>Right</button>").append("<br/>").append("<button onclick='clickButton(this.id)' style='margin-left:20px'  id='Down'>Down</button>")
  var video= $('<img src="http://' + window.location.host.split(":")[0] + ':8082/"/>')
        
        
  cheat_sheet.append(b).append(video)
  /*for (var s in state){
    cheat_sheet.append($('<li>').html(s+'<a class="type" onclick="loadHelp(\''+state[s].type+'\')" href="#">'+state[s].type+'</span>'))
  }*/
  
  //tr.append($('<td style="vertical-align: top;">').append(code))
  tr.append($('<td class="cheatsheet">').append("<h3>Movimientos ruedas</h3>").append(cheat_sheet)).attr('width: 500px;')
  
  $('#dialog #content').html(table)
 	
		var editor

	
	var editor=this.editor
	$(editor).focus()
	
	$(editor).find('.CodeMirror-scroll').attr('height: 100%;')

 
}
// var botones_accept_configure = function(){
//   this.params=this.getParams()
//   this.update()
// }

//var botones_deactivate = function(){}

var BotonesAction=extend(Action, {paramOptions:[{type:String,text:'arriba',name:'arriba'},{type:String,text:'nombre',name:'nombre',show:true}]})

// BotonesAction.prototype.configure=function(){
//   Node.prototype.configure.call(this);
// }
//BotonesAction.prototype.acceptConfigure=botones_accept_configure
BotonesAction.prototype.activate=showInterface
//BotonesAction.prototype.deactivate=botones_deactivate

//BotonesAction.prototype.setName("HOLA");
/*
BotonesAction.prototype.paint=function(options){
  Node.prototype.paint.call(this, options)
  
  $('#'+this.id+' text#param').text("HOLA")
  //alert('#'+this.id+' text#param')
  
  return this.svggroup
}*/



main.behaviour.nodeFactory.add('botones',BotonesAction)

})();
function clickButton(id){
	//$.post("lua/",{"exec":"bla","button":id});
  main.lua_exec('manager.notify('+this+')')
	// en el servidor se asume que el "handler" de este botón es el nodo con nombre: nodo_[id]
	// osea, si id="Up", el handler es el nodo "nodo_up"
}