Main=function(){
	if ( ! (this instanceof Main) )
		throw("Main is a class, not a function")
	
	this.server_ready = true
	this.canvas=new Canvas()
	this.behaviour=new Behaviour(this.canvas)
	this.connecting_dialog=false
	this.initUpdates()
	this.lua_ready = true
	

	
}

/**
 * @short Initializes polling to update state from server (e.g. LUA feedback, active nodes, etc.)
 * 
 */
Main.prototype.initUpdates = function() {
  var that = this
  $.get('/update/?state', that.updateState) 
  that.checkServer();
}

/**
 * @short checks server state; when server recovers from a crash, reinitializes updates
 * 
 */
Main.prototype.checkServer = function() {
//   var that = this
//   $.ajax({
//     url:'check',
//     type:'HEAD',
//     complete: function(xhr, text) {  
//       setTimeout(function(){
//         console.log(xhr.status+text)
//         if (xhr.status != 404) { // server not ready
// 	  that.server_ready = false
// 	  $('#loading').show()
// 	  that.checkServer()
// 	} else if (!that.server_ready) { // server ready but just recovered
// 	  that.server_ready = true
// 	  $('#loading').hide()
// 	  window.onbeforeunload=function(){}
// 	  document.location='http://'+document.location.hostname+':8081'
// 	} else that.checkServer(); // server ok
//       },1000) 
//     }
//   })
}

/**
 * @short updates general state, active/inactive nodes and Lua output
 * 
 */
Main.prototype.updateState = function (state) {
  var that = this
  var behaviour = main.behaviour
  
//   if(state.startStop) {
//     var btn=$('#startstop')
//     if ($('#startstop.play').length && state.startStop == 'on'){
// 	    btn.removeClass('play').addClass('stop')
// 	    btn.find('img').attr('src','static/img/mainstop.png').attr('title',current_language.stopclick).attr('style','height:auto; width:auto;')
//     }
//     else if ($('#startstop.stop').length && state.startStop == 'off'){
// 	    btn.removeClass('stop').addClass('play')
// 	    btn.find('img').attr('src','static/img/play.png').attr('title',current_language.playclick).attr('style','height:auto; width:auto;')
//     }
//   }
  
  if(state.active)
    for (var id in state.active)
      behaviour.state[id].activate()
	
  if(state.inactive)
    for (var id in state.inactive)
      behaviour.state[id].deactivate()
  
  if(state.luaOutput) { 
    var text = state.luaOutput
    var pre=$('#lua_console pre')
    pre.append(text)
  
   }
   
  $.get('/update/?state', main.updateState) 
}


/**
 * @short Show The dialog. Has several options:
 * 
 * title - The title of the dialog, by default, stays as it is
 * onclose - Which function to call on close
 * content - html to set at content
 */
Main.prototype.showDialog = function(opts){
	$('#overwindow').fadeIn('fast')
	
	this.closeDialog=this.closeDialogDefault
	$('#dialog #custom_buttons').html('')
	if (opts){
		if (opts.onclose || opts.onClose)
			this.closeDialog=opts.onclose || opts.onClose
		if (opts.title)
			$('#dialog #title').text(opts.title)
		if (opts.content)
			$('#dialog #content').html(opts.content)
	}
	$('#dialog #buttons').show()
}

Main.prototype.hideDialog = function(){
	$('#overwindow').fadeOut('fast')
}

/**
 * @short Close current dialog applying the changes
 */
Main.prototype.closeDialogDefault = function(){
	this.hideDialog()
}

Main.prototype.dialogNextOrClose = function(e){
	if (e.which==13){
		var next=$('#dialog ul #'+(Number(this.id)+1))
		if (next.length==0)
			main.closeDialog()
		next.focus()
		//e.disableEvent()
	}
}

/**
 * @short Removes all current data, optionally asking for confirmation
 */
Main.prototype.newBehaviour = function(force){
	if (!force && !confirm(current_language.newbehaviour))
		return;

	if ($('#startstop.stop').length)
	  main.startStop(true);
      
	this.behaviour.clear()

	if(!force)
	  this.behaviour.setMetaData();
}


Main.prototype.startStop = function(silent){
	var that = this
	var behaviour = this.behaviour
	var btn=$('#startstop')
	
	if ($('#startstop.play').length){
		btn.find('img').attr('src','img/mainstop.png').attr('title',current_language.stopclick).attr('style','height:auto; width:auto;')
		setTimeout(function(){
		      $.post("/manager/",{run:1}, function(){
			    btn.removeClass('play').addClass('stop')
		      }).error(function(){
			    btn.find('img').attr('src','img/play.png').attr('title',current_language.playclick).attr('style','height:auto; width:auto;')
			    if(!silent)
			      alert(current_language.playerror);
		      })
	      }, 1000)  
		
	} else if ($('#startstop.stop').length){
		$.post("/manager/",{stop:1}, function(){
			btn.removeClass('stop').addClass('play')
			btn.find('img').attr('src','img/play.png').attr('title',current_language.playclick).attr('style','height:auto; width:auto;')		  
		}).error(function(){
		      if(!silent)
			alert(current_language.stoperror);
		})
	}
}


Main.prototype.showMessage = function(msg, timeout){
	var d=$('<div id="messagebox">')
	d.attr('style','top:'+($('#svgscroll').position().top+1)+'px;')
	d.text(msg).hide()
	$('body').append(d)
	d.slideDown()
	if (timeout){
		setTimeout(hideMessage,timeout)
	}
}

Main.prototype.hideMessage = function(){
	$('#messagebox').slideUp(function(){ $('#messagebox').remove() })
}


Main.prototype.showHelp = function(){
  if($('#help').is(":visible"))
    this.hideHelp()
  else
    $('#help').fadeIn()
}

Main.prototype.hideHelp = function(){
  $('#help').fadeOut()
}

Main.prototype.loadHelp = function(section){
  $('#ihelp').attr('src','doc/'+section+'.html')
  this.showHelp();
}

Main.prototype.backHelp = function(){
  window.frames[0].history.back()
}

Main.prototype.showConsole = function(){
  if($('#console').is(":visible"))
    this.hideConsole()
  else $('#console').fadeIn()
}

Main.prototype.hideConsole = function(){
  $('#console').fadeOut()
}

Main.prototype.showFiles = function(){
  if($('#webdav').is(":visible"))
    main.hideFiles()
  else $('#webdav').fadeIn()

}

Main.prototype.hideFiles = function(){
  $('#webdav').fadeOut()
}

Main.prototype.reloadConsole = function(){
  $('#iconsole').attr('src','../stderr')
}

Main.prototype.hideLUAConsole = function(){
  $('#lua_console').fadeOut()
}

Main.prototype.showLUAConsole = function(){
    if($('#lua_console').is(":visible"))
      main.hideLUAConsole()
    else $('#lua_console').fadeIn()
}

Main.prototype.executeLUA = function(){
  var expr=$('#lua_console input').val()
  $('#lua_console pre').append('\n\n>>> '+expr+'\n')
  main.lua_exec(expr)
  $('#lua_console input').select().focus()
}

Main.prototype.lua_exec = function(cmd){
	var that = this
	if(that.lua_ready) {
	  that.lua_ready = false
	  $('#loading').show()  
	  $.post('/lua/',{exec:cmd},function(data){
			$('#lua_console pre').append('\n'+data+'\n')
	    that.lua_ready = true
	    $('#loading').hide()
	  }).error(function(){
		  alert("Error executing LUA code");
	  })
	}
}

// Main.prototype.js_node_notify_enter = function(id){
//   this.behaviour.get(id).activate()
// }
// 
// Main.prototype.js_node_notify_exit = function(id){
//   this.behaviour.get(id).deactivate()
// }


Main.prototype.setupGUI = function(){
  
  var canvas=this.canvas
  $(window).resize(function(){canvas.updateCanvasSize(canvas)})
  this.canvas.updateCanvasSize(canvas)

  $('[lid]').each(function(){
    var el=$(this)
    var lid=el.attr('lid')
    if (current_language[lid])
      el.text(current_language[lid])
    else
      console.log("Missing lid: "+lid)
  })
  
  // Add currently used GUI main buttons titles in proper language (default is English, see index.html):
  $('newb').title = current_language.newb_title
  $('metadatab').title = current_language.metadatab_title
  $('saveb').title = current_language.saveb_title
  $('loadb').title = current_language.loadb_title
  $('actionsb').title = current_language.actionsb_title
  $('eventsb').title = current_language.eventsb_title
  
  $('view_reset').title = current_language.view_reset_title
  $('view_all').title = current_language.view_all_title
  $('helpb').title = current_language.helpb_title
  $('luab').title = current_language.luab_title
  
  //setTimeout(function(){canvas.updateCanvasSize(canvas)}, 1000)
	  
  $('#ihelp').load(function(){ $('#ihelp').contents().find('body').append($('<style>@import "../css/help.css";</style>')) })
  
  var that=this
  $('#svgcanvas').text('')
  $('#svgcanvas').svg({ onLoad:function(svg_){that.canvas.setupGraph(svg_,that.canvas);} })
   
  window.onbeforeunload=function(){
    if ($('#startstop.stop').length)
      main.startStop(true);
    
    $.post("/manager/",{save:0})
    return current_language.before_unload;
    
  }
  
  $('#upload_xml').ajaxForm(function() {
    
	    $('#loading').hide()
	    //if (!confirm(current_language.shownewbehaviour)) return;
	    main.refresh(true);
	    
	    var fl = $('#upload')
	    var nfl=$('<input type="file" name="upload" id="upload" onchange="main.load()">')
	    fl.replaceWith(nfl)
   })
  
}

Main.prototype.load = function(){

  if ($('#startstop.stop').length)
    main.startStop(true);
      
  $('#loading').show()
      
  $('#upload_xml').submit()
  
}

Main.prototype.files = function(){

}

Main.prototype.refresh = function(force){
  
  if ($('#startstop.stop').length)
    main.startStop(true);
      
  var behaviour = this.behaviour;
  var canvas = this.canvas;
  $.get('/manager/?refresh', function(plain_xml){
      if(force) {
	behaviour.clear(true);
      }
      // Here, parse xml and update canvas, to be properly sync with server
      var xml=$($.parseXML(plain_xml))
  
    xml.find('event').each(function(){
      var ev=$(this)
      var type=ev.attr('type')
      var id=ev.attr('id')
      var param=[]
      var position={}
      ev.find('param').each(function(){
	pOpts = behaviour.nodeFactory.get(type).prototype.paramOptions
	for( var p in pOpts) {
	  if( pOpts[p].name == $(this).attr('name') ) {
	     if( pOpts[p].type == Number || pOpts[p].type == Array )
	      param[$(this).attr('name')]=Number($(this).text())
	    else
	      param[$(this).attr('name')]=$(this).text()
	  }
	}
      })
      if (ev.attr('x')){
	position.x=Number(ev.attr('x'))
	position.y=Number(ev.attr('y'))
      }
      behaviour.addNode(type, id, param, position, true)
    })
    xml.find('action').each(function(){
      var act=$(this)
      var type=act.attr('type')
      var id=act.attr('id')
      var param=[]
      var position={}
      act.find('param').each(function(){
	pOpts = behaviour.nodeFactory.get(type).prototype.paramOptions
	for( var p in pOpts) {
	  if( pOpts[p].name == $(this).attr('name') ) {
	    if( pOpts[p].type == Number || pOpts[p].type == Array )
	      param[$(this).attr('name')]=Number($(this).text())
	    else
	      param[$(this).attr('name')]=$(this).text()
	  }
	}
      })
      if (act.attr('x')){
	position.x=Number(act.attr('x'))
	position.y=Number(act.attr('y'))
      }
      
      behaviour.addNode(type, id, param, position, true)
    })
    xml.find('connection').each(function(){
      var c=$(this)
      var l=behaviour.connect(behaviour.state[c.attr('from')],behaviour.state[c.attr('to')],'#555',c.attr('id'))
      c.find('guard').each(function(){
	var guard=$(this)
	var first=guard.text().indexOf("[")+1;
	var last=guard.text().indexOf("]");
	l.setGuard(guard.text().substring(first,last))
      })
    })
     xml.find('meta').each(function(){
	var ev=$(this)
	ev.find('name').each(function(){
	  behaviour.name=this.textContent
	})
	ev.find('description').each(function(){
	  behaviour.description=this.textContent
	})
	ev.find('viewpoint').each(function(){
	  canvas.setViewpoint(this.textContent)
	})
      })  
    
  }, 'text')
  
}


Main.prototype.save = function(){
	var that = this;
	
	this.canvas.updateServer()

	if (this.behaviour.name==""){
		this.behaviour.setMetaData(this.save);
		return;
	}
		
	if(that.behaviour.name != "" && that.behaviour.ready) { 
		$('#loading').show()
		that.behaviour.ready=false
	  var iframe = document.getElementById("hiddenDownloader");
	  iframe.src = "/data/"+that.behaviour.name+".ab"; 
	  $('#loading').hide()
	  that.behaviour.ready=true
	} 
}


$(document).ready(function(){
  main=new Main()
  main.setupGUI();
  
  var timerId=setInterval(function(){
    if(main.behaviour.ready && main.canvas.ready) { 
      clearInterval(timerId)
      main.refresh();
      main.canvas.changeTool('events')
      document.body.style.cursor = 'default'
    }
  },500);
  
  // Some sanity to show user if connected to server, or not.
  var loadingDone=function(){ 
    if (main.connecting_dialog){                                         
      main.connecting_dialog=false; 

      var btn=$('#startstop')
      btn.removeClass('loading').addClass('play')
      btn.find('img').attr('src','img/play.png').attr('title', current_language.loadtitle).attr('style','height:auto; width:auto;')
      //hideDialog(); 
    }
  }
  var loadingError=function(){
    if (!main.connecting_dialog){
      main.connecting_dialog=true
      var btn=$('#tools #startstop')
      btn.find('img').attr('src','img/loading.gif').attr('title',current_language.connection_lost).attr('style','width: 50; height: 50;')
      var btn=$('#startstop').removeClass('stop').removeClass('play').addClass('loading')
    }
  }
  
})


/// Some fixes, only for debugging

// From http://stackoverflow.com/questions/690781/debugging-scripts-added-via-jquery-getscript-function
// Replace the normal jQuery getScript function with one that supports
// debugging and which references the script files as external resources
// rather than inline.
jQuery.extend({
   getScript: function(url, callback) {
      var head = document.getElementsByTagName("head")[0];
      var script = document.createElement("script");
      script.src = url;

      // Handle Script loading
      {
         var done = false;

         // Attach handlers for all browsers
         script.onload = script.onreadystatechange = function(){
            if ( !done && (!this.readyState ||
                  this.readyState == "loaded" || this.readyState == "complete") ) {
               done = true;
               if (callback)
                  callback();

               // Handle memory leak in IE
               script.onload = script.onreadystatechange = null;
            }
         };
      }

      head.appendChild(script);

      // We handle everything using the script element injection
      return undefined;
   },
});

