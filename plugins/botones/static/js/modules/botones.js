/**
 */

(function()
{
var python_configure = function(){
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
  var canvas= $('<canvas id="canvas" width="450" height="368"></canvas>') 
  var butvideo= $('<button id="photo">Take a Photo!</button>')  
        
        
  cheat_sheet.append(b).append(video).append(canvas).append(butvideo)
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

  (function camara2()
  {
    var is_playing = true,
    canvas_1 = $('#canvas'),
   /* canvas_2 = $('#canvas_2'),
    canvas_3 = $('#canvas_3'),
    canvas_4 = $('#canvas_4'),
    */cxt_1 = canvas_1[0].getContext('2d'),
   /* cxt_2 = canvas_2[0].getContext('2d'),
    cxt_3 = canvas_3[0].getContext('2d'),
    cxt_4 = canvas_4[0].getContext('2d'),
   */ video = $('#video'),
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
    $('#photo').click
    (
      function()
      {
        is_playing = !is_playing;
      }
      );
    function setTone(context, canvas, color_unchange)
    {
      var imgData = context.getImageData(0, 0, canvas.width, canvas.height),
      pixels = imgData.data,
      i = 0,
      grayscale;
      for (i, n = pixels.length; i < n; i += 4) {
        grayscale = pixels[i] * .3 + pixels[i+1] * .59 + pixels[i+2] * .11;
        switch (color_unchange) {
          case red:
          pixels[i+1] = grayscale;
          pixels[i+2] = grayscale;
          break;
          case green:
          pixels[i ] = grayscale;
          pixels[i+2] = grayscale;
          break;
          case blue:
          pixels[i ] = grayscale;
          pixels[i+1] = grayscale;
          break;
          default:
          pixels[i ] = grayscale;
          pixels[i+1] = grayscale;
          pixels[i+2] = grayscale;
          break;
        }
      }
      context.putImageData(imgData, 0, 0);
    }
    window.setInterval
    (
      function tolcolor()
      {
        if (is_playing) {
          cxt_1.drawImage(video, 0, 0, 450, 368);
          /*cxt_2.drawImage(video, 0, 0, 450, 368);
          cxt_3.drawImage(video, 0, 0, 450, 368);
          cxt_4.drawImage(video, 0, 0, 450, 368);
          */setTone(cxt_1, canvas_1[0]);
          /*setTone(cxt_2, canvas_2[0], red);
          setTone(cxt_3, canvas_3[0], green);
          setTone(cxt_4, canvas_4[0], blue);
        */}
      },
      100
      );
  })();
}
python_accept_configure = function(){
  //this.params['code']=this.editor.getValue()
  this.update()
}

var PythonAction=extend(Action)

PythonAction.prototype.configure=python_configure
PythonAction.prototype.acceptConfigure=python_accept_configure




main.behaviour.nodeFactory.add('botones',PythonAction)



})();
function clickButton(id){
	$.post("lua/",{"exec":"bla","button":id});
	// en el servidor se asume que el "handler" de este botón es el nodo con nombre: nodo_[id]
	// osea, si id="Up", el handler es el nodo "nodo_up"
}
