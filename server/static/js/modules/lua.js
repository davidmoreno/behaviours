/**
*/

(function(){
  var lua_configure = function(){
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


var lua_configureEvent = function(){
	var that = this
  this.configureDialogSetup()
  var p=this.paramOptions
  var ul=$('<ul>')
  var li=$('<li>')
  ul.append(li)
  li.text(current_language.repeat)
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
  li.text(current_language.repeat)
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

lua_accept_configure = function(){
  
  this.params=this.getParams()
  this.params['code']=this.editor.getValue()
  this.update()
}
lua_realtime_update = function(){
  this.params=this.getParams()      
}

lua_getParams = function(){
  var that= this
  var p=this.paramOptions
  if (p){
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

              if(val==0){

                $('image#nodeonoff'+this.id).attr('href','img/on.png')
              }
              else{
                $('image#nodeonoff'+this.id).attr('href','img/off.png')
              }

            }
            if(p[i].name=="noderepeat"){
              if(val==11)                            
                $('text#noderepeat').text("Always")              
              else
              $('text#noderepeat').text(""+val)
            }
          }
        }
      }
      return params;
    }
    return {}

  }

  lua_update=function(){
    if (!this.params)
      return
    var txt=[]
    for(var i=1;i<3;i++){
      txt.push(this.paramOptions[i].values[this.params[this.paramOptions[i].name]])
      if(this.id.indexOf(this.type)==-1){
        if(this.paramOptions[i].name=="nodeon") {
          if(this.paramOptions[i].values[this.params[this.paramOptions[i].name]]=="NO"){


            $('image#nodeonoff'+this.id).attr('href','img/off.png')
          }   
          else{

            $('image#nodeonoff'+this.id).attr('href','img/on.png')
          }
        }
      }
    }
  }

  var LUAAction=extend(Action, {paramOptions: [{type:Text,text:current_language.lua_action_msg,name:'code'}]})
  var LUAEvent=extend(Event, {paramOptions: [{type:Text,text:current_language.lua_event_msg,name:'code'},{type:Array,values:['YES','NO'],name:'nodeon'},
   {type:Array,values:['Never','01','02','03','04','05','06','07','08','09','10','Always'],name:'noderepeat'}]})

  LUAAction.prototype.configure=lua_configure
  LUAAction.prototype.acceptConfigure=lua_accept_configure


  LUAEvent.prototype.configure=lua_configureEvent
  LUAEvent.prototype.acceptConfigure=lua_accept_configure
  LUAEvent.prototype.realtime_update=lua_realtime_update
  LUAEvent.prototype.update=lua_update
  LUAEvent.prototype.getParams=lua_getParams

  main.behaviour.nodeFactory.add('lua_action',LUAAction)
  main.behaviour.nodeFactory.add('lua_event',LUAEvent)
})()
