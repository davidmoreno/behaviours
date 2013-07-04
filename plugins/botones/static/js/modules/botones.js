/**
 */

(function()
{
var showInterface = function(){
  this.configureDialogSetup()

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
  var video= $('<video id="video" width="450" height="368" autoplay="autoplay"></video>')
        
        
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

  var camara = function()
  {
    var is_playing = true,
    video = $('#video'),
    video = video[0],
    red = 1,
    green = 2,
    blue = 3;
    if (navigator.getUserMedia) {
      navigator.getUserMedia(
        { 'video': true },
        function(stream)
        {
          video.src = stream;
          video.play();
        }
        );
    } else if (navigator.webkitGetUserMedia) {
      navigator.webkitGetUserMedia
      (
        { 'video': true },
        function(stream)
        {
          video.src = window.webkitURL.createObjectURL(stream);
          video.play();
        }
        );
    } else if (navigator.mozGetUserMedia) {
      navigator.mozGetUserMedia
      (
        { 'video': true },
        function(stream)
        {
          video.mozSrcObject = stream;
          video.play();
        },
        function(err)
        {
          alert('An error occured! '+err);
        }
        );
    }
  }
  camara();
}
var botones_accept_configure = function(){
  this.update()
}

var botones_deactivate = function(){}

var BotonesAction=extend(Action)

BotonesAction.prototype.configure=showInterface
BotonesAction.prototype.acceptConfigure=botones_accept_configure
BotonesAction.prototype.activate=showInterface
BotonesAction.prototype.deactivate=botones_deactivate




main.behaviour.nodeFactory.add('botones',BotonesAction)



})();
function clickButton(id){
	$.post("lua/",{"exec":"bla","button":id});
	// en el servidor se asume que el "handler" de este botón es el nodo con nombre: nodo_[id]
	// osea, si id="Up", el handler es el nodo "nodo_up"
}
