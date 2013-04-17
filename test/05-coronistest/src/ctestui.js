CTestUI = function(){

	ctestui=this

	this.followCommand=false

	var filename = function(file){
		return file.replace(/\./g,'_').replace(/\-/g,'_').replace(/ /g,'_').replace(/\//g,'_')
	}

	/**
	* @short Creates a modal window over all the other content.
	*/
	this.createOverWindow = function(greyid, content){
		mscreen=window.screen
		mheight=mscreen.height
		mwidth=mscreen.width

		var overwindow=$('<div/>')
		overwindow.attr('id',greyid)
		overwindow.addClass('hidden')
		overwindow.html(content)

		var grey=$('<div/>')
		grey.attr('id',greyid+'_back')
		grey.addClass('hidden')
		grey.bind('click',function(){
			overwindow.toggleClass('hidden')
			overwindow.toggleClass('overwindow')
			grey.toggleClass('hidden')
			grey.toggleClass('greywindow')
		})

		$('body').append(grey)
		$('body').append(overwindow)
	}

	/// Shows/hides the command panel
	this.toggleCommandPanel = function(){
		$('#command_back').click()
	}

	/// Performs scroll so that child is viewable
	this.scrollTo = function(_p, _c){
		var p=_p[0]
		var c=_c[0]
		var pos=0
		while(c!=p){
			pos+=c.offsetTop
			c=c.offsetParent
			if (!c)
				break;
		}
		pos=pos-(_p.height()/2)
		p.scrollTop=pos
	}

	/// Shows/hides the log panel
	this.toggleLog = function(){
		$('#log_back').click()
		this.scrollTo($('#log'),$('#progress_text :last'))
	}

	/// toggles the breakpoint on a tr.
	this.toggleBreakpoint = function(){
		t=$(this)
		sf=$('#control #fileselector').attr('value')
		line=t.children('th').text()
		if (t.hasClass('breakpoint')){
			t.removeClass('breakpoint')
			ctest.delBreakpoint(sf,line)
		}
		else{
			t.addClass('breakpoint')
			ctest.addBreakpoint(sf,line)
		}
	}

	/**
	 * @short Parses a data, ad returns a list of sentences
	 */
	this.showData = function(data,origFile){
		file=filename(origFile)

		if (!$('#cmds_'+file)[0])
			$('#control #fileselector').append($('<option>').attr('value',origFile).text(origFile))

		var cmds=data.split('\n')
		var ret=[]
		$(cmds).each(function(i){ 
			var str=String(this)
			ret.push(str)
		})

		var depth=ctest.stack.length

		id='cmds_'+file
		var tble=$('#'+id)
		if (tble.length){
			tble.empty()
		}
		else{
			tble=$('<table/>')
			tble.attr('id','cmds_'+file)
		}
		tble.append($('<th colSpan="2"/>').append($('<a>').attr('href',origFile).text(origFile)))

		$.each(ret, function(i){
			var tr=$('<tr/>')
			var th=$('<th>').text(i+1)
			tr.append(th)
			var td=$('<td/>')
			pre=$('<pre/>')
			pre.text(String(this))
			pre.attr('id','cmd_'+file+'_'+(i+1))
			td.append(pre)
			tr.append(td)
			tr.click(ctestui.toggleBreakpoint)
			tble.append(tr)
		})
		tble.addClass('hidden')

		$('#commandbox').append(tble)

		this.showCurrentSourceFile()

		return ret
	}

	/**
	 * @short Shows just the current selected file
	 */
	this.showCurrentSourceFile = function(){
		var sf=$('#control #fileselector').attr('value')
		var file=filename(sf)
		//ctestui.debug('Selected file: '+sf)
		var me=$('#commandbox #cmds_'+file)
		if (me.hasClass('hidden')){
			$('#commandbox table').addClass('hidden')
			me.removeClass('hidden')
		}
	}


	/// Sets the status to the given cmd line. They have only one status (running, done, error)
	this.setCommandStatus = function(cmd, stats){
		if (!cmd || cmd.length<4)
			return
		var file=filename(cmd[2])
		var line=cmd[3]
		var l=$('#cmd_'+file+'_'+line)
		l.removeClass()
		l.addClass(stats)
		if (!$('#command_back').hasClass('hidden') && 
			(cmd[2]==$('#control #fileselector').attr('value')))
				this.scrollTo($('#commandbox'),l)
	}

	/// Clears the status of all commands, done when starting a new run
	this.clearCommandStatus = function(){
		$('#commandbox pre').removeClass('done')
		$('#commandbox pre').removeClass('doing')
		$('#commandbox pre').removeClass('error')
	}

	this.setCommandComment = function(cmd, txt){
		if (!cmd || cmd.length<4)
			return
		var file=filename(cmd[2])
		var line=cmd[3]
		var l=$('#cmd_'+file+'_'+line)
		l.attr('title',txt)
	}

	/// Ensures that the given command is visible: change source file and focus
	this.showCommand = function(cmd){
		if ($('#command_back').hasClass('hidden'))
			$('#command_back').click()

		var file=cmd[2]
		if (file=='_internal_')
			return;
		var line=cmd[3]
		$('#control #fileselector').attr('value',file)
		ctestui.showCurrentSourceFile()
		var l=$('#cmd_'+filename(file)+'_'+line)
		if (l.length)
			ctestui.scrollTo($('#commandbox'),l)
	}

	/// A new URL was loaded
	this.newURL = function(url){
		this.log('Loaded url '+url)
		$('#showurl').text(url).attr('href',url)
		location.hash=url
	}

	/// Show all current vars and values at the log
	this.showVars = function(){
		this.log('Showing vars: ')
		for (v in vars){
			this.log(v+'='+vars[v],'vars')
		}
	}

	/**
	* @short Log some progress, with an optional class (error...) to the log console
	*/
	this.log = function(txt, klass){
		var msg=$('<pre></pre>')
		var id=''
		if (ctest){
			var cp=[]
			$(ctest.stack).each(function(i){
				cp.push(this.cp)
			})
			id=cp.join('.')
		}
		msg.text(id+' - '+String(txt))
		msg.addClass('status')
		if (klass)
			msg.addClass(klass)
		msgc=msg.clone()
		$('div#progress_text').append(msgc)
		if (klass!='debug')
			$('#status').html(msg)
	}

	/// Basic log with debug info, sometimes just ignored-
	this.debug = function(txt){
		ctestui.log(txt,'debug')
	}

	/// Logs the runnign of a command. 
	this.logCommand = function(cmd){
		var _vars=[]
		$(cmd[1]).each(function(){
			if (this[0]=='v')
				_vars.push(this[1])
			else
				_vars.push("'"+this[1]+"'")
		})
		txt=cmd[0]+'('+_vars.join(', ')+')'
		this.log(txt,'cmd')
	}

	/// Sets the follow command variable.
	this.switchFollowCommand = function(){
		this.followCommand=$('#follow:checked').length>0
	}

	/// Runs the command at #runcommand, and adds it to history if succesfull.
	/// Its not a very powerfull implementation, as it may continue execution if already on pause. FIXME.
	this.runCommand = function(){
		var t=$('input#runcommand')

		var val=t.attr('value')
		ctestui.log('Running '+val)
		if (val[0]=='#'){
			ctestui.addCommand(val)
			return false;
		}
		try{
			ctest.resetVM()
			ctest.loadCustomData(val)
			ctest.run()
			ctest.atEnd=function(){
			  t.select()
			  t.focus()
				ctestui.addCommand(val)
			}
		}
		catch(e){
			if (e.txt)
			  alert('Error running code: '+e.txt)
			else
			  alert('Error running code: '+e)
		}
		return false;
		t.select()
		t.focus()
	}

	this.resize = function(){
		if ($('textarea.tall').length){
			var h=$(window).height()
			h-=$('#header').height()+$('#customcommand').height()+105
			$('textarea.tall').height(h)
		}
		else{
			$('textarea').height(60)
			var h=$(window).height()
			h-=$('#header').height()+$('#control').height()+$('#customcommand').height()+105+$('textarea').height()
			$('#commandbox').height(h)
		}
	}

	/// Initializes the gui
	this.startup = function(){
		$(document).shortkeys({
			's':function(){ ctest.runStep() },
			'c':function(){ ctest.run() },
			'g':function(){ ctest.doGo() },
			'p':this.toggleCommandPanel,
			'l':this.toggleLog,
			'v':this.showVars
		})
		//this.createOverWindow('command', $('#command_panel'))
		this.createOverWindow('log', $('#progress_text'))

		$('form#runcommand').submit(this.runCommand)

		$('#fileselector').change(this.showCurrentSourceFile)

		$(window).resize(this.resize)
		this.resize()
		
		if (location.hash){
			try{
				$('iframe').attr('src',location.hash.substr(1))
			}
			catch(e){
			}
		}
	}

	this.addCommand = function(val){
		var ta=$('#runhistory textarea')
		ta.append(val+'\n')
		ta[0].scrollTop=ta[0].scrollHeight
		
	}
	
	this.startup()
}

