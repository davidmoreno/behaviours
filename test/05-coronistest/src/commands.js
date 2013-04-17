/**
 * @short Searchs for an element in the contained page
 */
$$$ = function(_id){ 
	var id=_id
	// presearch
	var noregex = function(t){ // basic remove regexp.. normally its just this.
		return t.replace(".*","")
	}
	
	if (id.substr(0,5)=='link='){
		var ltext=id.substr(5)
		id='a:contains("'+noregex(ltext)+'")'
	}
	else if (id.substr(0,7)=='option='){
		id='option:contains("'+noregex(id.substr(7))+'")'
	}

	//ctestui.log(id)
	// normal search
	var element=$('iframe').contents().find(id) 

	// postsearch, filter a bit more, get real link, not aproximately
	if (_id.substr(0,5)=='link='){
		var el=[]
		$.each(element, function(i){
			thistext=$(this).text().replace(/[ ]*$/,'').replace(/^[ ]*/,'')
			if (thistext.match(RegExp('^'+ltext+'$')))
				el.push(this)
		})
		element=$(el)
	}


	if (!element.length){
		throw( { may_appear_later : true, text : 'Element not found', 'element_name' : _id } )
		throw(s)
	}

	return element
}

/**
 * @short Injects some data into the guest page
 */
injectData = function(data){
	ctestui.debug('injecting data: '+data)
	$$$('body').append(data)
}

/**
 * @short Injects some script into the guest page
 *
 * It does in a quite dirty way and maybe it will show bugs in the future. 
 * It just calls the single expression as if the parent were frames[0] (the iframe).
 */
injectScript = function(data){
	ctestui.debug('injecting javascript: '+data)
	with(frames[0].window){
		eval(data) // easiest and ugliest execute. 
	}
}

/**
 * @short A python like dir, returns the list of attributes as text
 *
 * This is a debugger helper, not really needed by ctest, but to develop ctest-
 */
dir = function(a){
	r=[]
	for (var i in a)
		r.push(i)
	return r.sort()
}

/**
 * @short All the commands that can be run. 
 *
 * If you want to add more, its just a dictionary, with do<command(arguments)>
 */
commands = {
	/// Type something inside a input
	'type' : function(element, txt){
		//ctestui.log('Element: '+element)
		//ctestui.log('txt: '+txt)
		$$$(element).attr('value',txt)
	}
	,
	'check' : function(element){
		$$$(element).attr('checked',true)
	}
	,
	'uncheck' : function(element){
		$$$(element).attr('checked',false)
	}
	,
	/// Do a click on some element. Only the first!
	'click' : function(element, wait){
		alreadyClicked=false
		$$$(element).first().each(function(){
			var t=$(this)
			var ev=$('iframe')[0].contentDocument.createEvent('MouseEvent')
			var offset=t.offset()
			var sx=offset.top;
			var sy=offset.left;
	
			var buttonpressed=0
			
			if ($.browser.msie)
				buttonpressed=1
			if ($.browser.mozilla){
				if (this.tagName=="A" && !alreadyClicked){ 
					ctestui.log('Opening HREF at mozilla: '+t.attr('href'))
					var js=t.attr('onclick')
					if (js){
						with( frames[0].window ){
							var ok=js()
							if (!ok)
								return;
						}
					}
					var _href=t.attr('href')
					if (_href.substr(0,11)=='javascript:')
						injectScript(_href.substr(11))
					else if (_href[0]!='#')
						commands.open(_href,false) // should not wait, none of the others wait.
					alreadyClicked=true
				}
				//ev=document.createEvent('HTMLEvent')
				ev.initEvent('click', false, false);
			}
			else
				ev.initMouseEvent("click", true, true, $('iframe')[0].contentWindow, 1, 
					sx, sy, 0, 0, 
					false, false, false, false, buttonpressed, null);
			//ev.initEvent('click',true,true)
			ctestui.log('click '+t)
			try{
				if (this.dispatchEvent)
					this.dispatchEvent(ev)
				else
					this.fireEvent(ev)
			}
			catch(e){
				ctestui.log('exception when clicking: '+String(e))
			}
		})
		if (wait){
			ctestui.debug('Extra waitLoad()')
			ctest.loadCustomData('waitLoad()')
		}
	}
	,
	/// Drag some object somewhere or over some object. Idea from https://github.com/jquery/jquery-ui/blob/master/tests/unit/draggable/draggable_core.js, MIT license
	'drag' : function(el, dx, dy) {
		
		var findCenter = function(el) {
			var o = el.offset();
			return {
				x: Math.floor(o.left + el.outerWidth() / 2),
				y: Math.floor(o.top + el.outerHeight() / 2)
			};
		}

		var element = $$$(el)
		if (!dy){
			var dest=$$$(dx)
			var co=findCenter(element)
			var cd=findCenter(dest)
			dx=cd.x-co.x
			dy=cd.y-co.y
		}
		element.simulate("mousedown")
		element.simulate("mousemove",{clientX:dx, clientY:dy})
		$$$('body').simulate("mouseup",{clientX:dx, clientY:dy})
		$$$('body').simulate("click",{clientX:dx, clientY:dy})
	}
	,
	/// Sets a new variable on the commands scope, by default text content
	'set' : function(variable, element, attribute){
		element=$$$(element)
		if (attribute)
			vars[variable]=element.attr(attribute)
		else
			vars[variable]=element.text()
	}
	,
	/// Do a regexp on an expression, and stores the first group on the given variable
	'regexp' : function(variable, orig,regexp){
		vars[variable]=orig.match(RegExp(regexp))[1]
	}
	,
	/// Sets a attribute value
	'setAttr' : function(element, attribute, val){
		element=$$$(element)
		element.attr(attribute, val)
	}
	,
	/// Checks an attribute exists, and has that value
	'checkAttr': function(element, attribute, value){
		try{
			element=$$$(element+'['+attribute+']')
		}
		catch(e){
			element=$$$(element)
		}
		vars['lasttext']=element.attr(attribute)
		if (value){
			if (!vars['lasttext'].match(value))
				throw( {'text':'Value does not match', 'may_appear_later': true} )
		}
		else{
			throw( {'text':'Attribute not found', 'may_appear_later': true} )
		}
	}
	,
	/// Writes something on the log
	'log' : function(text){
		ctestui.log(text)
	}
	,
	/// opens a given url
	'open' : function(text,wait){
		wait=(wait!=false)

		if (text[0]!='/'){
			var c=$('iframe')[0].contentWindow.location.href
			if (c[c.length-1]=='/')
				text=c+text
			else{ // must change the last part from /a/b/c with d, to /a/b/d
				text=c.substr(0,c.lastIndexOf('/'))+'/'+text
			}
		}

		ctestui.log('opening '+text)

		if (wait) 
			ctest.loadCustomData('waitLoad()')
		$('iframe')[0].contentWindow.location.href=text
	}
	,
	/// Verifies an element does not exist; this one does not wait to check if it does not exist after, but just now. 
	/// as a side effect it sets teh variable 'lasttext' to its text.
	'checkElement' : function(element){
		try{
			var el=$$$(element)
			vars['lasttext']=el.text()
		}
		catch(e){
			throw({ may_appear_later: true, text: "Element '"+element+"' not present" })
		}
	}
	,
	/// Waits until the page is loaded, uses a counter to check from last command it has loaded a page
	'waitLoad' : function(){
		if (!ctest.pageLoaded){
			//ctestui.log('keep waiting')
			throw( { 'wait_for_load' : true, may_appear_later : true, text : "Waiting for new page load" } )
		}
		ctestui.log('Page loaded accepted')
	}
	,
	/// Sets the state machine to ensure that there is an error on next command, if there is no error, throws it
	'errorOnNext' : function(){
		ctest.errorOnNext=1
	}
	,
	/// Checks if the given regexp is in the text, if no element is passed, assumes body
	'checkText' : function(txt, element){
		if (!element)
			element='body'
		if (!$$$(element).text().match(txt))
			throw( { may_appear_later : true, text : 'Text not found' } )
	}
	,
	/// Opens another file and executes its contents. The file location is relative to current file location
	'load' : function(file){
		ctest.runFile(file)
	}
	,
	// Waits for an alert. If some text given that text is used as regexp to match
	'waitAlert' : function(reg){
		if (!ctest.lastAlert){
			throw( { may_appear_later : true, text : 'No alert yet' } )
		}
		if (reg){
			re=RegExp(reg)
			if (!ctest.lastAlert.match(re))
				throw( { may_appear_later : false, text : 'Wrong alert found, waiting for'+reg+', found '+txt } )
		}
		ctest.lastAlert=false
		ctest.nextAlert=undefined
	}
	,
	// Waits for a prompt. If some text given that text is used as regexp to match
	'waitPrompt' : function(reg){
		if (!ctest.lastPrompt){
			throw( { may_appear_later : true, text : 'No prompt yet' } )
		}
		if (reg){
			re=RegExp(reg)
			if (!ctest.lastPrompt.match(re))
				throw( { may_appear_later : false, text : 'Wrong prompt found, waiting for'+reg+', found '+txt } )
		}
		ctest.lastPrompt=false
		ctest.nextPrompt=undefined
	}
	,
	// Waits for a confirmation. If some text given that text is used as regexp to match
	'waitConfirm' : function(reg){
		if (!ctest.lastConfirm){
			throw( { may_appear_later : true, text : 'No confirmation yet' } )
		}
		ctestui.log('confirm appeared with text: '+ctest.lastConfirm); 

		if (reg){
			re=RegExp(reg)
			if (!ctest.lastConfirm.match(re))
				throw( { may_appear_later : false, text : 'Wrong confirm found, waiting for'+reg+', found '+txt } )
		}
		ctest.lastConfirm=false
		ctest.nextConfirm=undefined
	}
	,
	// Prepares the system to receive an alert (only one) soon.
	'nextAlert' : function(){
		ctest.nextAlert=true
	}
	,
	// Sets the next confirmation
	'nextConfirm' : function(val){
		ctest.nextConfirm=val
	}
	,
	// Sets the next prompt response
	'nextPrompt' : function(val){
		ctest.nextPrompt=val
	}
	,
	// Concats several strings, and sets the variable at first arg.
	'concat' : function(arg, txt1, txt2, txt3, txt4){
		var t=txt1
		if (txt2)
			t+=txt2
		if (txt3)
			t+=txt3
		if (txt4)
			t+=txt4
		vars[arg]=t
	}
	,
	/// Selectes a value by text on a <select>
	'select' : function(el, txt){
		var el=$$$(el)
		var val=$$$('option='+txt).val()
		//ctestui.log('select '+el)
		el.val(val)
		var ev=$('iframe')[0].contentDocument.createEvent('HTMLEvents')
		ev.initEvent('change',true,true)
		el[0].dispatchEvent(ev)

		var oncl=el.attr('onchange')
		if (oncl){
			with( frames[0].window ){
				if (typeof(oncl)=='string')
					eval(oncl)
				else
					oncl()
			}
		}
	}
	,
	///
	/// Marks this element changing the background to yellow.
	'mark' : function(_el){
		if (vars['lastmark']){
			try{
				$$$(vars['lastmark']).attr('style','')
			}
			catch(e){
			}
		}
		var el=$$$(_el)
		el.attr('style','background: yellow')
		vars['lastmark']=_el
	},
	/// Shows an alert with the given text. Usefull for debugging. Can use variables (of course)
	'alert': function(txt){
		alert(txt)
	},
	/// Evals some javascript code
	'eval': function(js){
		eval(js)
	}
}

/// The variables as seen by the program. Please note there is no kind of local vars, not even parameters.
vars = { 'true': true, 'false': false }

/// Custom funcitons in ctest code
funcs = {}
