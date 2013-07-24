/**
 */

(function()
{
function clickButton(id){
  main.lua_exec('manager.notify('+id+')')
}

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

	var state=main.behaviour.state
  var buttonPanel= $('<div class="buttonsPanel">').css({'margin-left': 'auto','margin-right': 'auto','width':'200px'})
  var buttonUp = $('<button>').css({'margin-left':'30px'}).click(function(){clickButton(that.params['arriba'])}).attr('id','Up').html('Up');
  var buttonDown = $('<button>').css({'margin-left':'30px'}).click(function(){clickButton(that.params['abajo'])}).attr('id','Down').html('Down');
  var buttonLeft = $('<button>').css({'margin-left':'30px'}).click(function(){clickButton(that.params['izquierda'])}).attr('id','Left').html('Left');
  var buttonRight = $('<button>').css({'margin-left':'30px'}).click(function(){clickButton(that.params['derecha'])}).attr('id','Right').html('Right');
  buttonPanel.append(buttonUp).append($('<br>')).append(buttonLeft).append($('<br>')).append(buttonRight).append($('<br>')).append(buttonDown);
  var video= $('<img src="http://' + window.location.host.split(":")[0] + ':8082/"/>')
        
  var contenido = $("<div>").append("<h3>Movimientos ruedas</h3>").append(buttonPanel).append(video)
  $('#dialog #content').html(contenido)
}

var BotonesAction=extend(Action, {paramOptions:[
    {type:String,text:'arriba',name:'arriba'},
    {type:String,text:'abajo',name:'abajo'},
    {type:String,text:'izquierda',name:'izquierda'},
    {type:String,text:'derecha',name:'derecha'},
    {type:String,text:'nombre',name:'nombre',show:true,default:"Botones"}
  ]})

BotonesAction.prototype.activate=showInterface

main.behaviour.nodeFactory.add('botones',BotonesAction)

})();

