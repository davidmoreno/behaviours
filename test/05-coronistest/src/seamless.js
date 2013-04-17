/**
 * @short Allows to seamless use a web page and record events and selections to create a test suite.
 */

seamless_mark='seamless_'+Math.floor(Math.random()*10000000)

/**
 * @short Gets an element as in $$$, but marks it so its never again get the same (so no duplicate clicks, for example).
 */
var s$$$ = function(selector,extraid){
	var el= $.unique($$$(selector))
	el=el.filter(function(){
		if (this['seamless_mark'+extraid])
			return false;
		this['seamless_mark'+extraid]=true
		return true
	})
	return el
}

/**
 * @short Given an element tries to get a unique selector to identify it, or false
 */
guessSelector = function(element){
	var beautify=function(sel){
		/// beautify it a bit
		sel=sel.replace(/\[id=(.*)\]/,'#$1')
		sel=sel.replace(/^a:contains\((.*)\)$/,'link=$1')
		return sel
	}
	var isUnique = function(selector){
		if (!selector)
			return selector
		console.log(selector)
		try{
			var me=$$$(selector)
			if (me.length==1 && me.is(element))
				return selector
		}
		catch(e){
		}
		console.log("No")
		return false
	}
	
	var attrs=['id','name','class','alt','title']
	
	var tagname=element[0].tagName.toLowerCase()
	
	for (var i in attrs){
		var a=attrs[i]
		var sel=tagname+'['+a+'='+element.attr(a)+']'
		if (isUnique(sel))
			return beautify(sel)
	}
	
	// maybe by text inside
	var sel=tagname+':contains('+element.text()+')'
	if (isUnique(sel)){
		// Return it
		return beautify(sel)
	}
	
	
	console.log("Fail")
	return false;
}

/**
 * @All elements here are called at each page load.
 */
seamless = {
	click:function(){
		var logClick=function(e){
			var me=$(this)
			var selector=guessSelector(me)
			if (!selector){
				selector='link='+me.text()
			}
			if (selector)
				ctestui.addCommand('click("'+selector+'")')
		}
		
		var elements=['a','input','h1','h2','h3','th','td']
		var selector=[]
		for (var i in elements){
			selector.push(elements[i]+':not(:has(*))')
		}
		$.unique(s$$$(selector.join(',')),'click').click(logClick)
	}
	,
	selection:function(){
		/// By using tips from http://mark.koli.ch/2009/09/use-javascript-and-jquery-to-get-user-selected-text.html
		var getSelection = function(){
			var t = '';
			var w=window.frames[0]
			if(w.getSelection){
				t = w.getSelection();
			}else if(w.document.getSelection){
				t = w.document.getSelection();
			}else if(w.document.selection){
				t = w.document.selection.createRange().text;
			}
			return t.toString();
		}
		var mouseup = function(){
			var st = getSelection();
			if(st!=''){
				ctestui.addCommand('checkText("'+st+'")')
			}
		}
		
		$(window.frames[0]).bind("mouseup", mouseup);
	}
	,
	type:function(){
		var onchange=function(){
			var me=$(this)
			var selector=guessSelector(me)
			if (selector){
				ctestui.addCommand('type("'+selector+'","'+me.val()+'")')
			}
		}
		
		try{
			s$$$('input,textarea','type').change(onchange)
		}
		catch(e){
		}
	}
	,
	select:function(){
		var onchange=function(){
			var me=$(this)
			var selector=guessSelector(me)
			if (selector){
				var option=me.children('[value='+me.val()+']').text()
				ctestui.addCommand('select("'+selector+'","'+option+'")')
			}
		}
		
		s$$$('select').change(onchange)
	}
}

activateSeamless=function(){
	$('#control').slideUp()
	$('#commandbox').slideUp()
	$('textarea').addClass('tall')
	ctestui.resize()
	
	ctestui.log("Load seamlesses.")
	for (var s in seamless){
		ctestui.log("Apply seamless: "+s)
		try{
			seamless[s]()
		}
		catch(e){
			ctestui.log("There was an error activating seamless: "+s)
		}
	}
}

deactivateSeamless=function(){
	ctestui.log("Recording seamless is enabled until next page load.")
	ctestui.addCommand('# seamless deactivated')
	$('#control').slideDown()
	$('#commandbox').slideDown()
	$('textarea').removeClass('tall')
	ctestui.resize()
}
