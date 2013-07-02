/**
 */

(function(){
var python_configure = function(){
  this.configureDialogSetup()

  var tr=$('<tr>')
  var table=$('<table>').append(tr)
  var code=$('<textarea rows="20" id="code">')
  
  if (this.params.code)
    code.val(this.params.code)
    
//   if (this.params.exec)
//     code.val(this.params.exec)
//   if (this.params.check)
//     code.val(this.params.check)
    
  var cheat_sheet=$('<ul>')
	var state=main.behaviour.state
	var b= $('<div class="buttonsPanel">').css({'margin-left': 'auto','margin-right': 'auto','width':'200px'})

    b.append("<button onclick='clickButton(this.id)' style='margin-left:30px' id= 'Up'>Up</button>").append("<br/>").append("<button onclick='clickButton(this.id)' id='Left'>Left</button>").append("<button onclick='clickButton(this.id)' id='Right'>Right</button>").append("<br/>").append("<button onclick='clickButton(this.id)' style='margin-left:20px'  id='Down'>Down</button>")
	cheat_sheet.append(b)
  /*for (var s in state){
    cheat_sheet.append($('<li>').html(s+'<a class="type" onclick="loadHelp(\''+state[s].type+'\')" href="#">'+state[s].type+'</span>'))
  }*/
  
  //tr.append($('<td style="vertical-align: top;">').append(code))
  tr.append($('<td class="cheatsheet">').append("<h3>Movimientos ruedas</h3>").append(cheat_sheet)).attr('width: 500px;')
  
  $('#dialog #content').html(table)
 	
		var editor
	this.editor = CodeMirror.fromTextArea(code[0], {
		tabMode: "indent",
		matchBrackets: true,
		theme: "lesser-dark",
		extraKeys: {
				"F10": function() {
					$('.CodeMirror').toggleClass('CodeMirror-fullscreen')
					editor.refresh()
				},
				"Esc": function() {
					$('.CodeMirror').removeClass('CodeMirror-fullscreen')
					editor.refresh()
				}
		}
	});
	
	var editor=this.editor
	$(editor).focus()
	
	$(editor).find('.CodeMirror-scroll').attr('height: 100%;')
}

python_accept_configure = function(){
  this.params['code']=this.editor.getValue()
  this.update()
}

var PythonAction=extend(Action, {paramOptions: [{type:Text,text:current_language.python_action_msg,name:'code'}]})
var PythonEvent=extend(Event, {paramOptions: [{type:Text,text:current_language.python_event_msg,name:'code'}]})

PythonAction.prototype.configure=python_configure
PythonAction.prototype.acceptConfigure=python_accept_configure


PythonEvent.prototype.configure=python_configure
PythonEvent.prototype.acceptConfigure=python_accept_configure

main.behaviour.nodeFactory.add('python2action',PythonAction)
main.behaviour.nodeFactory.add('python2event',PythonEvent)


})()
function clickButton(id){
	alert(id)
}
