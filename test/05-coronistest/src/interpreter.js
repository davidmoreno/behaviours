/**
 * @simple interpreter for the code for ctest.
 *
 * 
 * It allows only method calling, and variable assignment, including assignment to functions.
 * 
 * @see LANGUAGE.TXT to see whats supported.
 */


/**
 * @short Interprets a file, and returns the code (c), from the source code (stream).
 *
 * It reads the stream (its a string), and looks for tokens, there are very few types. As they are obtained,
 * the code is interpreted and the code tree is generated. Finally we get something as this:

	{	'functions' : { 
			'login' : 
				{ 'args' : ['username','password'] ,
				'code' : [ 
						[ 'open', [ ['f','/log'] ] ],
						[ 'open', [ ['f','/i18n/setlang/?language=en&next=/'] ] ],
						[ 'type', [ ['f','#username'], ['v','username'] ] ],
						[ 'set', [ ['v','username'],['t','texto'] ] ]
					]
				} 
			} ,
		'code' : [
			['load', [ ['t', 'users.ctest'] ]
		]
	}
 
 *
 * 
 */
CTestInterpreter = function(stream,file){
	var c={ 'code' : [], 'functions' : [] }

	var streamPosition=0
	var line=1
	var lastToken=''
	var re_SimpleChar=/[\(\)=,\{\}#]/
	var re_Separator=/[ \n\t]/
	var re_String=/["']/
	var re_Id=/[a-zA-Z]/
	var re_IdCont=/[a-zA-Z0-9_]/

	/// Just consumes one character
	getChar = function(){
		if (streamPosition>=stream.length)
			throw ({'eof':true})
	
		var r=stream[streamPosition]
		streamPosition+=1
		//ctest.log('read '+r)
		return r
	}
	/**
	 * @short Reads a token from the current stream
	 * 
	 * It returns a list [type, token].
	 * 
	 * Type is:
	 * 
	 * c - char
	 * v - variable
	 * t - quoted string 
	 */
	readToken = function(){
		/// Gets a string, starts with ' or " and finished with the same. \ is escape.
		getString = function(startWith){
			var ret=''
			var c=''
			do{
				ret+=c
				c=getChar()
				if (c=='\\'){
					c=getChar()
				}
			}while (c!=startWith)
			return ret
		}
		/// Gets an Id, It receives the first letter
		getId = function(startWith){
			var ret=startWith
			while(true){
				c=getChar()
				if (c.match(re_IdCont))
					ret+=c
				else{
					streamPosition-=1
					return ret
				}
			}
		}

		try{
			var c=getChar()
			while (c.match(re_Separator)){
				if (c=='\n')
					line+=1
				c=getChar()
			}
		}
		catch(e){
			if (e.eof){
				lastToken='EOF'
				return lastToken
			}
			throw(e)
		}

		lastToken='UNKNOWN'
		if (c.match(re_SimpleChar))
			lastToken=['c',c]
		else if (c.match(re_String))
			lastToken=['t',getString(c)]
		else if (c.match(re_Id))
			lastToken=['v',getId(c)]


		//ctest.log(lastToken)
		return lastToken
	}
	// Reads to the end of the line, normally on comments
	readLine = function(){
		var c=getChar()
		var ret=''
		while (c!='\n'){
			ret+=c
			c=getChar()
		}
		line+=1
		return ret
	}


	/// Throws an exeption that shows a syntax error or similar
	error=function(txt){
		throw({'error':txt, 'line':line, 'streamPosition':streamPosition,'lastToken':lastToken})
	}

	
	///  The grammar 
	///

	/// Reads an instruction line, returns it
	readInstruction = function(token){
		if (token[0]=='c' && token[1]=='#'){
			ctestui.log(readLine())
			return false
		}

		if (token[0]!='v')
			error('Expected identifier')
		if (token[1]=='javascript'){
			return ['eval',[ ['t',readBlock()] ],file,line]
		}
		id=token[1]
		token=readToken()
		if (token[0]!='c')
			error('Expected "=" (function assignment) or "(" (function call)')
		if (token[1]=='='){
			createFunction(id)
			return false;
		}
		var args=[]
		if (token[1]!='(')
			error('Expected "=" (function assignment) or "(" (function call)')

		token=readToken()
		while (token[0]!='c' && token[1]!=')'){
			if (token[0]!='v' && token[0]!='t')
				error('Expecting variable name or text as argument')
			args.push(token)
			token=readToken()
			if (token[0]!='c')
				error('At function call I was waiting for a "," or ")"')
			if (token[1]==',')
				token=readToken()
			else if (token[1]!=')')
				error('At function call I was waiting for a "," or ")"')
		}

		if (id=='load'){
			joinpath = function(path, file){
				var r=file;
				if (path)
					r=path+'/'+file
				return r.replace('//','/').replace('//','/')
			}
			dirname = function(file){
				var dn=file.replace(/[^/]*$/,'')
				return dn;
			}
			var fle=args[0][1]
			if (fle[0]!='/'){
				fle=joinpath(dirname(file),fle)
				args[0][1]=fle
			}
			ctestui.log('Loading another file: '+fle)
			ctest.loadFile(fle)
		}

		//ctest.log($.toJSON(ret))
		return [id, args, file, line]
	}

	/// Reads a block between { and } and returns it. It can have subblocks.
	readBlock = function(){
		var token=readToken()
		if (token[0]!='c' && token[1]!='{')
			error('Expecting { to open a block')
		
		var block='{\n'
		var depth=1
		
		while (depth>0){
			var l=readLine()
			for (var i in l){
				if (l[i]=='{')
					depth++;
				else if (l[i]=='}')
					depth--;
			}
			block+=l+'\n';
		}
		
		return block
	}

	/// Reads a new function
	createFunction = function(id){
		token=readToken()
		if (token[0]!='v' && token[1]!='function')
			error('At function definition missing "function" keyword')
		token=readToken()
		if (token[0]!='c' && token[1]!='(')
			error('At function definition missing "(" for argument definition')

		token=readToken()
		var args=[]
		while (token[0]!='c' && token[1]!=')'){
			if (token[0]!='v')
				error('Expecting variable name as function argument definition')
			args.push(token[1])
			token=readToken()
			if (token[0]!='c')
				error('At function definition I was waiting for a "," or ")"')
			if (token[1]==',')
				token=readToken()
			else if (token[1]!=')')
				error('At function definition I was waiting for a "," or ")"')
		}

		token=readToken()
		if (token[0]!='c' && token[1]!='{')
			error('At function definition missing "{" after "function(...)" keyword')
		token=readToken()

		var code=[]
		do{
			ins=readInstruction(token)
			if (ins)
				code.push(ins)

			token=readToken()
		}while(token[0]!='c' && token[1]!='}')
		
		ctestui.log('added function '+id)
		c['functions'][id]={'code':code, 'args':args}
	}

	//file=file.replace('.','_').replace('-','_').replace(' ','_')

	try{
		var token=readToken()
		do{
			var ins=readInstruction(token)
			if (ins)
				c['code'].push(ins)

			token=readToken()
		}while(token!='EOF')
	}
	catch(e){
		if (e.error){
			txt=e.error+"\nLine: "+e.line+"\nRead token: "+$.toJSON(e.lastToken)
			alert(txt)
		}
		else
			throw(e)
	}

	return c;
}

