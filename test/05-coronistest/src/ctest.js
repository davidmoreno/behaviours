CTest = function(){
	/// Code by file
	this.code={}
	/// Current stack. It has a dict in each position with:
    /// 'cp'   -- code pointer, 
    /// 'fp'   -- file pointer, filename at this.code, 
	/// 'code' -- the code to execute
	this.stack=[]

	/// If there has been a page loaded between last command finalization and current command
	this.pageLoaded=false
	/// If we should switch error reporting on next step: and error is ok, no error is bad. 
	/// 0 - Error no tolerable
	/// 1 - Waiting for error
	this.errorOnNext=0
	/// url history of loaded pages, sometimes usefull to debug
	this.urlHistory=[]

	// last issued alert, should be null unless waiting for it.
	this.lastAlert=false
	// last issued alert, should be null unless waiting for it.
	this.lastPrompt=false
	// last issued alert, should be null unless waiting for it.
	this.lastConfirm=false
	/// Answer for the next prompt
	this.nextPrompt=undefined
	/// Answer for the next confirm
	this.nextConfirm=undefined

	/// Milliseconds to wait between retries, usually 100 retries
	this.stepSpeed=25
	// How many commands passed
	this.commandCount = 0
	// If running just now
	this.running=false
	/// The function to execute on next round. CTest is based on a polling on this function, if !false
	this.functionForNextRound=false
	/// Initial time
	this.initialTime=(new Date()).getTime()
	/// Breakpoints known
	this.breakpoints={}
	/// Skip next breakpoint: it has already stopped, and next command is a step or continue so shoul dignore the first bp
	this.ignoreNextBreakpoint=false
	/// What to do, if any, at end
	this.atEnd=false

	var ctest=this

	/**
	 * @short Checks the state of the stack
	 *
	 * If end of current stack, return to old one
	 */
	this.checkStack = function(){
		var depth=ctest.stack.length-1
		var codelength=ctest.stack[depth].code.length

		// pop a level from stack, if 0 stacks end
		if (ctest.stack[depth].cp>=codelength){ 
			ctestui.debug('Poping from stack')
			ctest.stack.pop()

			// nothong on stack, not waiting for anything
			if (ctest.stack.length==0){
				ctest.stopExecuting('End of tests',true)
				return
			}
			return this.checkStack()
		}
	}

	/// Current depth, where to point on stacked structures
	this.currentDepth = function(){
		return ctest.stack.length-1
	}

	this.step = function(){
		ctest.updateTime()
		ctest.checkStack() // might be changes of stack

		var depth=ctest.currentDepth()
		var cmd=ctest.stack[depth].code[ctest.stack[depth].cp]

		// Commands to execute if executing a normal function succeed
		var nextStep = function(){
			ctestui.debug('nextStep at level '+depth+', last took '+(100-ctest.notHereCounter)+' tries')
			ctestui.setCommandStatus(cmd, 'done')
			ctest.commandCount+=1
			$('#command_count').text(ctest.commandCount)
			ctest.stack[depth].cp+=1
			if (ctest.running)
				ctest.setFunctionForNextRound(ctest.step)
		}
		// Commands to execute on failure
		var failure = function(e){
			if (e.element_name)
				e="Element '"+e.element_name+"' not found"
			if (e.wait_for_load)
				e="Waiting to long to load page"
			var msg=""
			if (e.text)
				msg=String(e.text)
			else
				msg=String(e)
			// error on all levels up to get here, on prev instruction (the one that made the call)
			for (i=0;i<depth;i++){ 
				var mcmd=ctest.stack[i].code[ctest.stack[i].cp-1]
				ctestui.setCommandStatus(mcmd, 'error')
				ctestui.setCommandComment(mcmd, msg)
			}
			// and on current isntr.
			var mcmd=ctest.stack[depth].code[ctest.stack[depth].cp]
			ctestui.setCommandStatus(mcmd, 'error')
			ctestui.setCommandComment(mcmd, msg)
			ctestui.showCommand(mcmd)
			ctest.stack[depth].cp+=1

			ctest.stopExecuting(msg,false)
		}

		ctestui.logCommand(cmd)

		ctestui.setCommandStatus(cmd, 'doing')
		if (ctest.errorOnNext!=1)
			ctest.doInstruction(cmd, nextStep, failure)
		else{
			ctest.doInstructionError(cmd, nextStep, failure)
		}
	}

	/**
	 * @short Just executes the command, like an eval
	 */
	this.execCommand = function(command){
		/// Translates a parameter from ['v','var'] to get the value from where it is or ['t','hello'] to hello.
		var t = function(param){
			if (param[0]=='t'){ // basic replacements of ${vars}
				var mvars=param[1].match(/\${[^}]*}/g)
				var r=param[1]
				for(var v in mvars){
					v=mvars[v]
					r=r.replace(v,vars[v.substr(2,v.length-3)])
				}
				return r
			}
			else if (param[0]=='v' && (param[1] in vars))
				return vars[param[1]]
			else
				throw ({may_appear_later:false, text:'Unknown parameter for command: '+command[0]})
		}

		var cmd=commands[command[0]]
		if (!cmd)
			cmd=funcs[command[0]]
		if (!cmd){
			throw({may_appear_later:false, text:'Command not known: '+command[0]})
		}
		if (cmd.code){
			$(cmd.args).each(function(i){
				vars[this]=t(command[1][i])
			})
			//ctestui.log('rnu:'+ $.toJSON(cmd.code))
			ctest.loadCustomCode(cmd.code)
		}
		else{
			var ret
			//ctestui.log('Command: '+cmd)
			if (command[1].length==0)
				ret=cmd()
			else if (command[1].length==1)
				ret=cmd(t(command[1][0]))
			else if (command[1].length==2)
				ret=cmd(t(command[1][0]),t(command[1][1]))
			else if (command[1].length==3)
				ret=cmd(t(command[1][0]),t(command[1][1]),t(command[1][2]))
			else if (command[1].length==4)
				ret=cmd(t(command[1][0]),t(command[1][1]),t(command[1][2]),t(command[1][3]))
			else if (command[1].length==5)
				ret=cmd(t(command[1][0]),t(command[1][1]),t(command[1][2]),t(command[1][3]),t(command[1][4]))
			//ctestui.log('Ret! '+ret)
			return ret
		}
	}


	/// Performs a single instruction; or more it tries to do it many times until succeed
	/// When sucesfull, calls onSuccess, or on failure calls onFailure.
	this.doInstruction = function(command, onSuccess, onFailure){
		//ctestui.log('do instruction: '+$.toJSON(command))
		ctest.notHereCounter=100
		if (ctestui.followCommand)
			ctestui.showCommand(command)

		if (ctest.running && !ctest.ignoreNextBreakpoint &&
			ctest.breakpoints[command[2]] && ctest.breakpoints[command[2]][command[3]]) {
			ctestui.log('Breakpoint!')
			ctestui.showCommand(command)
			ctest.ignoreNextBreakpoint=true
			ctest.running=false
			return;
		}
		ctest.ignoreNextBreakpoint=false

		// Just do my command many times.
		doMyCommand = function(){
			//ctestui.debug('docommand try '+ctest.notHereCounter+', command '+command)
			try{
				//ctestui.debug('>doeval -- '+'commands.do'+$.toJSON(command))
				ctest.execCommand(command)
				//ctestui.debug('<doeval -- '+'commands.do'+$.toJSON(command))
			}
			catch(e){ // errors
				//ctestui.debug('!<doeval -- '+'commands.do'+$.toJSON(command))
				// first option, may appear later, keep trying.
				if ( e.may_appear_later && (ctest.notHereCounter>0) ){
					ctest.notHereCounter-=1
					ctest.setFunctionForNextRound(doMyCommand)
					return;
				}
				if (!e.may_appear_later)
					ctestui.log('Error that is not solvable later')
				// call the failure option
				onFailure(e)
				throw(e)
			}
			ctest.pageLoaded=false
			onSuccess()
		}
		doMyCommand()
	}

	/// Performs a single instruction; or more it tries to do it many times until succeed
	/// This version looks for an error, if no error keeps trying until error.
	this.doInstructionError = function(command, onSuccess, onFailure){
		ctest.notHereCounter=100
		ctestui.log('do instruction error '+command)

		// Just do my command many times.
		doMyCommand = function(){
			ctestui.log('docommand error try '+ctest.notHereCounter+', command '+command)
			try{
				ctest.execCommand(command)
			}
			catch(e){ // errors
				// first option, may appear later, keep trying.
				// call the failure option
				ctest.pageLoaded=false
				ctest.errorOnNext=0
				onSuccess()
				return
			}
			if ( ctest.notHereCounter>0 ){
				ctest.notHereCounter-=1
				ctest.setFunctionForNextRound(doMyCommand)
				return;
			}
			onFailure({text:'No error appeared'})
			throw(e)
		}
		doMyCommand()
	}

	/// Do the running
	this.run = function(){
		ctest.running=true
		ctest.errorOnNext=false
		ctest.step()
	}

	/// Stops it, performs current step and finishes
	this.stop = function(){
		ctest.running=false
	}


	/// updates the time label
	this.updateTime = function(){
		currnt=(new Date()).getTime()
		elapsed=Math.floor((currnt-ctest.initialTime)/1000.0)
		s=String("0"+elapsed%60).substr(-2)
		m=String("0"+Math.floor(elapsed/60)).substr(-2)
		$('.time_elapsed').text(m+':'+s)
	}

	/**
	 * @short Loads an external file
	 */
	this.loadFile = function(filename){
		$.ajax( {
				url     : filename,
				success : function(data){ ctest.loadData(data,filename); },
				error   : function(){ 
						alert('Could not open test file '+String(filename))
					},
				dataType: 'text'
			} )
	}

	/// Gets a string with all commands, and create the proper level of data, and set the stack pointer there.
	this.loadData = function(data, file){
		ctestui.log("Adding data to stack")
		ctestui.showData(data,file)

		var code=new CTestInterpreter(data, file)
		
		for (cmd in code['functions']) // load functions. all are global.
			funcs[cmd]=code['functions'][cmd]

		ctest.code[file]=code.code
	}

	/// Loads custom code.
	this.loadCustomData = function(data){
		ctestui.log('Load custom code to stack')
		ctest.loadData(data,'_internal_')
		ctest.runFile('_internal_')
	}

	/// Same as load data, but data is forced by the application; ie do not come from a loading
	this.loadCustomCode = function(code){
		ctestui.log('Load custom code to stack')
		ctest.code['_internal_']=code
		ctest.runFile('_internal_')
	}

	/// Runs a given filename
	this.runFile = function(filename){
		if (!(filename in ctest.code)){
			ctest.stopExecuting('File '+filename+' never loaded. Cant continue.')
			//throw({may_appear_later:false, text:'File "'+filename+'" never loaded, can not continue'})
		}
		this.stack.push({'cp':0,'fp':filename,'code':ctest.code[filename]})
	}

	/**
	* @short Loads the suite file, and perform the actions
	*/
	this.doGo = function(runNow){
		ctest.resetVM()
		ctestui.clearCommandStatus()

		var file=$('#load').attr('value')
		ctest.runFile(file)

		ctest.run()
	}

	/**
	 * @short Sets the status of the VM at 0, no errors, empty stack..
	 */
	this.resetVM = function(){
		ctest.initialTime=(new Date()).getTime()

		ctest.stack=[]

		ctest.loadPageNumber=0
		ctest.loadPageNumberLastCommand=0

		$('body').removeClass('all_test_passed')
		$('body').removeClass('failed_tests')

		ctest.commandCount=0
	}

	/**
	 * @short Starts a execution if needed and executes a step
	 */
	this.runStep = function(){
		ctestui.log('Running a simple step')
		if (ctest.stack.length==0){
			ctest.resetVM()
			ctest.runFile($('#control #fileselector').attr('value'))
		}
		ctest.running=false
		ctest.step()
	}

	/// Adds a breakpoint
	this.addBreakpoint = function(file, line){
		if (!ctest.breakpoints[file])
			ctest.breakpoints[file]={}
		ctest.breakpoints[file][line]=true
		ctest.breakpoints['__latest__']={'file':file, 'line':line} // I do not use normal format, special data
	}

	/// Deletes a breakpoint
	this.delBreakpoint = function(file, line){
		if (!ctest.breakpoints[file])
			return
		delete ctest.breakpoints[file][line]
	}
	
	/// Starts the execution from where user set the latest breakpoint
	ctest.runFromLatestBreakpoint = function(){
		ctest.resetVM()

		var line=ctest.breakpoints['__latest__'].line
		var filename=ctest.breakpoints['__latest__'].file
		var code=ctest.code[filename]
		var cp=-1
		ctestui.debug('Start at '+filename+':'+line)
		// first check on normal files
		$(code).each(function(i){
			if (this[3]==line)
				cp=i
		})
		// now check functions
		if (cp<0){ 
			for (var f in funcs){
				var mycode=funcs[f].code
				$(mycode).each(function(i){
					ctestui.debug($.toJSON(this))
					if (this[3]==line && this[2]==filename){
						ctestui.debug('HERE!')
						cp=i
						code=mycode
						$(funcs[f].args).each(function(){
							vars[this]=prompt('Value for argument '+this)
						})
					}
				})
			if (cp>=0)
				break;
			}
		}
		if (cp<0){
			alert('Cant start execution from there, as there is no executable code there')
			return
		}

		this.stack.push({'cp':cp, 'fp':filename, 'code':code})
		ctest.ignoreNextBreakpoint=true // ignore myself

		ctest.run()
	}

	/**
	 * @short Gets the options from location.search, and returns a dictionary with them
	 */
	this.getPageOptions = function(){
		ret={}
		$.each(document.location.search.replace('?','').split('&'), function(i){
			a=String(this).split('=')
			ret[a[0]]=a[1]
		})
		return ret
	}

	/**
	 * @short Stops executing and sets the given error
	 */
	this.stopExecuting = function(msg, ok){
		if (console && console.trace)
			console.trace()
		ctest.running=false
		if (ok){
			$('body').first().addClass('all_test_passed')
			ctestui.log(msg)
			if (ctest.atEnd)
				ctest.atEnd()
		}
		else{
			$('body').first().addClass('failed_tests')
			ctestui.log(msg,'error')
		}
		ctest.runningCycle=false
		throw ({'stop':true})
	}


	this.runningCycle=false
	this.doCycle = function(){
		//ctestui.debug('...........................................')
		fnc=ctest.functionForNextRound;
		ctest.functionForNextRound=false;
		try{
			if (fnc)
				fnc()
			if (ctest.functionForNextRound)
				ctest.runningCycle=setTimeout(ctest.doCycle,ctest.stepSpeed)
			else{
				ctestui.debug('===============normal=end==============================')
				ctest.runningCycle=false
			}
		}
		catch(e){
			if (e.stop){
				ctest.runningCycle=false
				ctestui.debug('===============end=exception================='+ctest.notHereCounter)
			}
			else
				throw(e)
		}
	}

	/**
	 * @short Sets the function that will be executed on next cycle. If no cycle just now it initiates it.
	 */
	this.setFunctionForNextRound = function(fnc){
		ctest.functionForNextRound=fnc;
		if (!ctest.runningCycle){
			ctest.runningCycle=setTimeout(ctest.doCycle,ctest.stepSpeed)
		}
	}

	/**
	* @short startup
	*/
	this.startup = function(){
		$('#go').bind('click',ctest.doGo)

		// ready!
		$('div#progress_text').empty()
		//ctestui.log('Ready')

		$('iframe').load(function(){
			url=this.contentWindow.location.href
			ctestui.newURL(url)
			ctest.urlHistory.push(url)
			ctest.pageLoaded=true

			// Overwrite default bhaviour of some browser parts
			//ctestui.log('alert status: '+ctest.lastAlert+' / '+ctest.nextAlert)
			if (ctest.lastAlert && ctest.nextAlert==undefined){
				ctestui.log(printStackTrace().join('\n'))
				ctest.stopExecuting('There was an unmanaged alert: '+ctest.lastAlert)
			}

			// 2010 09 22 -- all ctest instances were parent.ctest. Dont know exactly why.
			this.contentWindow.alert=function(txt){
				ctestui.log('alert: '+txt); 
				if (ctest.lastAlert){
					ctestui.log(printStackTrace().join('\n'))
					stopExecuting('There was an unmanaged alert, and appeared a new one: '+ctest.lastAlert)
				}
				ctest.lastAlert=txt; 
			}
			if (ctest.lastPrompt && ctest.nextPrompt==undefined)
				ctest.stopExecuting('There was an unmanaged prompt: '+ctest.lastPrompt)
			this.contentWindow.prompt=function(txt){ 
				ctestui.log('prompt: '+txt); 
				if (ctest.lastPrompt){
					ctestui.log(printStackTrace().join('\n'))
					stopExecuting('There was an unmanaged prompt, and appeared a new one: '+ctest.lastPrompt)
				}
				ctest.lastPrompt=txt; 
				return ctest.nextPrompt;
			}
			if (ctest.lastConfirm && ctest.nextConfirm==undefined){
				ctestui.log(printStackTrace().join('\n'))
				ctest.stopExecuting('There was an unmanaged confirmation: '+ctest.lastconfirm)
			}
			this.contentWindow.confirm=function(txt){ 
				if (ctest.nextConfirm==undefined){
					ctestui.log(printStackTrace().join('\n'))
					stopExecuting('There was an unmanaged confirmation, and appeared a new one: '+ctest.lastconfirm)
				}
				ctestui.log('confirm: '+txt); 
				ctestui.log('confirm answer: '+ctest.nextConfirm); 
				if (ctest.lastConfirm){
					ctestui.log(printStackTrace().join('\n'))
					stopExecuting('There was an unmanaged confirmation, and appeared a new one: '+ctest.lastconfirm)
				}
				ctest.lastConfirm=txt; 
				return ctest.nextConfirm;
			}
			if ($('#seamless:checked').length){
				activateSeamless()
			}
		})

		var opts=this.getPageOptions()
		if ('go' in opts){
			setTimeout(this.doGo, 2000); // Go in 2s, so I have time to load everything. FIXME! Timeout?? Come on!
		}
		if ('load' in opts){
			$('#load').attr('value',opts['load'].replace(/%2F/g,'/'))
		}

		$('#apply_seamless').click(function(){
			activateSeamless()
		})
		$('#seamless').change(function(){
			if ($('#seamless:checked').length)
				activateSeamless()
			else
				deactivateSeamless()
		})
		
		ctest.loadFile($('#load').attr('value'))
	}

	this.startup()
	return this

}
/*
oldalert=window.alert
window.alert = function(txt){
	ctestui.log('ALERT: '+String(txt))
	ctestui.log(printStackTrace().join('\n'))
	oldalert('ALERT')
	oldalert(txt)
}
*/