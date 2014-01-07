require(['node','jquery','main','extra/codemirror','extra/codemirror_python'],function(node, $, main, CodeMirror){
/**
 */
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
  for (var s in state){
    cheat_sheet.append($('<li>').html(s+'<a class="type" onclick="loadHelp(\''+state[s].type+'\')" href="#">'+state[s].type+'</span>'))
  }
  
  tr.append($('<td style="vertical-align: top;">').append(code))
  tr.append($('<td class="cheatsheet">').append("<h3>"+current_language.current_objects+"</h3>").append(cheat_sheet))
  
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

var python_configure_event = function(){
  var that = this
  this.configureDialogSetup()
  var p=this.paramOptions
  var ul=$('<ul>')
  var li=$('<li>')
  ul.append(li)
  li.text("Activate event")
  var i=1
  var cb=$('<select>')
  for (var j in p[i].values){
    var opt=$('<option>')
    opt.append(p[i].values[j])
    opt.attr('value',j)
    cb.append(opt)
  }
  cb.change(function(){
    that.realtime_update_base()
  })
  cb.attr('id',i)
  cb.val(this.params[p[i].name])
  li.append(cb)

  var li=$('<li>')
  ul.append(li)
  li.text("Times repeat event")
  var i=2
  var cb=$('<select>')
  for (var j in p[i].values){
    var opt=$('<option>')
    opt.append(p[i].values[j])
    opt.attr('value',j)
    cb.append(opt)
  }
  cb.change(function(){
    that.realtime_update_base()
  })
  cb.attr('id',i)
  cb.val(this.params[p[i].name])
  li.append(cb)

  this.configureDialogSetup()

  var tr=$('<tr>')
  var table=$('<table>').append(tr)
  var code=$('<textarea rows="20" id="code">')
  
  if (this.params.code)
    code.val(this.params.code)
    
  var cheat_sheet=$('<ul>')
	var state=main.behaviour.state
  for (var s in state){
    cheat_sheet.append($('<li>').html(s+'<a class="type" onclick="loadHelp(\''+state[s].type+'\')" href="#">'+state[s].type+'</span>'))
  }
  
  tr.append($('<td style="vertical-align: top;">').append(code))
  tr.append($('<td class="cheatsheet">').append("<h3>"+current_language.current_objects+"</h3>").append(cheat_sheet))
  
  $('#dialog #content').html(table)
 $('#dialog #content').append(ul)	
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
  //this.params=python_getParams()
  this.update()
}
python_accept_configure_event = function(){
  //this.params['code']=this.editor.getValue()
  this.params=this.getParams()
  this.params['code']=this.editor.getValue()
  this.update()
}
python_realtime_update = function(){
  this.params=this.getParams()      
}

python_getParams = function(){
  var that= this
  var p=this.paramOptions
  if (p){
    var dialog=$('#dialog:visible')
    if (dialog.length==1){
    var params={}
    for(var i in p){
      if(this.id.indexOf(this.type)==-1){
      if(i==0){
        
        params[p[i].name]=that.editor.getValue()
      }
        else{
          var val=$('#dialog #content #'+i).val()
          params[p[i].name]=val 
          

            if(p[i].name=="nodeon"){
              if(this.changeactivity==true){
                val=this.changevalor
                params[p[i].name]=val 
                
              }
              if(val==0){

                $("#"+this.id+" g").attr('fill','#aad400')
                $("#"+this.id+" #legend").attr('fill','#000000')
                $("#"+this.id+" #param").attr('fill','#000000')
                $("#noderepeat"+this.id).attr('fill','#000000')                
                $('image#nodeonoff'+this.id).attr('href','img/on.png')
              }
              else{
                $("#"+this.id+" g").attr('fill','#C0C0C0')
                $("#"+this.id+" #legend").attr('fill','#666666')
                $("#"+this.id+" #param").attr('fill','#666666')
                $("#noderepeat"+this.id).attr('fill','#666666')
                $('image#nodeonoff'+this.id).attr('href','img/off.png')
              }

            }
            if(p[i].name=="noderepeat"){
               if(!val){
              val=$("#"+this.id+" #noderepeat"+this.id).text()
            }
              if(val==11)                            
                $('text#noderepeat'+this.id).text("Always")              
              else
              $('text#noderepeat'+this.id).text(""+val)
            }
          }
        }
      }
      return params;
    }
    return this.params
    }
    return {}

  }

  python_update=function(){
    if (!this.params)
      return
    var txt=[]
    for(var i=1;i<3;i++){
      txt.push(this.paramOptions[i].values[this.params[this.paramOptions[i].name]])
      if(this.id.indexOf(this.type)==-1){
        if(this.paramOptions[i].name=="nodeon") {
          if(this.paramOptions[i].values[this.params[this.paramOptions[i].name]]=="NO"){


            $("#"+this.id+" g").attr('fill','#C0C0C0')
                $("#"+this.id+" #legend").attr('fill','#666666')
                $("#"+this.id+" #param").attr('fill','#666666')
                $("#noderepeat"+this.id).attr('fill','#666666')
                $('image#nodeonoff'+this.id).attr('href','img/off.png')
          }   
          else{

            $("#"+this.id+" g").attr('fill','#aad400')
                $("#"+this.id+" #legend").attr('fill','#000000')
                $("#"+this.id+" #param").attr('fill','#000000')
                $("#noderepeat"+this.id).attr('fill','#000000')                
                $('image#nodeonoff'+this.id).attr('href','img/on.png')
          }
        }
        if(this.paramOptions[i].name=="noderepeat"){
        if(this.paramOptions[i].values[this.params[this.paramOptions[i].name]]==11)                            
          $('text#noderepeat'+this.id).text("Always")              
        else
          $('text#noderepeat'+this.id).text(""+this.paramOptions[i].values[this.params[this.paramOptions[i].name]])
      }
      }
    }
  }





python_accept_configure = function(){
  this.params['code']=this.editor.getValue()
  this.update()
}


var PythonAction=extend(Action, {paramOptions: [{type:Text,text:current_language.python_action_msg,name:'code'}]})
var PythonEvent=extend(Event, {paramOptions: [{type:Text,text:current_language.python_event_msg,name:'code'},{type:Array,values:['YES','NO'],name:'nodeon'},
   {type:Array,values:['Never','01','02','03','04','05','06','07','08','09','10','Always'],name:'noderepeat'}]})


PythonAction.prototype.configure=python_configure
PythonAction.prototype.acceptConfigure=python_accept_configure


PythonEvent.prototype.configure=python_configure_event
PythonEvent.prototype.acceptConfigure=python_accept_configure_event
 PythonEvent.prototype.realtime_update=python_realtime_update
  PythonEvent.prototype.update=python_update
  PythonEvent.prototype.getParams=python_getParams
main.behaviour.nodeFactory.add('python2action',PythonAction)
main.behaviour.nodeFactory.add('python2event',PythonEvent)
})
