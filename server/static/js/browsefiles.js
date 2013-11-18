/**
 * @short Create a file browser
 * 
 * Known options:
 * 
 * title
 * finish -- function to call after creation of dialog
 * open -- what to call when trying to open a file
 * show_files -- whether to show files. default true.
 * accepts -- what to do when accept button is closed. Must return true (close) or false (keep open)
 */
BrowseFiles = function(opts){
	if ( ! (this instanceof BrowseFiles) )
		throw("BrowseFiles is a class, not a function")
	var that=this
		
	this.path=""
	this.onopen=opts.open
	this.files=$('<div id="files">')
	this.show_files=opts.show_files==undefined ? true : opts.show_files
	
	$('#dialog #content').html(this.files)
	var tt=$('#dialog #title').text(opts.title)
	
	this.updateContents("/")
	
	main.showDialog({
		onclose : function(){
			if (opts.accept(that.path))
				main.closeDialogDefault()
		}
	})
	
	if (opts.finish)
		opts.finish()
}

BrowseFiles.prototype.open = function(file){
	if (file.type=='dir')
		this.chdir(file.filename)
	else{
		this.onopen(file,this.path)
		var newpath=this.path+"/"+file.filename;
		$.post('/upload/',{files:file.filename,path:this.path.slice(1,this.path.length)})
	}
}

BrowseFiles.prototype.updateContents = function(newpath){
	this.files.addClass("loading")
	var that=this
	$.get('/data/browse',{path:newpath},function(files){
		that.path=newpath
		console.log('Chdir to '+that.path)
		
		var fs=$('<table class="files">')
		var row=$('<tr>')
		row.append($('<th>').text(current_language.filename))
		row.append($('<th>').text(current_language.type))
		fs.append(row)
		
		
		$(files.files).each(function(){
			var file=this
			if (!that.show_files && file.type=="file")
				return
			var row=$('<tr>')
			var a=$('<a href="#_">').text(file.filename).click(function(){ that.open(file) })
			
			row.append($('<td>').append(a))
			row.append($('<td>').text(file.type))
			
			fs.append(row)
		})
		
		that.files.removeClass("loading")
		var h1=$('<h2>')

		if (newpath.length>1){
			var ps=newpath.split('/')
			var a=$('<a href="#_">').text("[root]").click(function(){ that.open({filename:'/',type:'dir'}) })
				h1.append(a)
			var path=''
			for(var i=0;i<ps.length;i++){
				if (ps[i]=='')
					continue
				h1.append('/')
				path=path+'/'+ps[i]
				var a=$('<a href="#_">').text(ps[i]).click(function(path){ return function(){ that.open({filename:path,type:'dir'}) } }(path)).attr('title',path)
				h1.append(a)
			}
		}

		
		that.files.html(h1).append(fs)
	},'json').error(function(){
		that.files.removeClass("loading")
		alert('Cant chdir to '+newpath)
	})
}

BrowseFiles.prototype.chdir = function(path){
	if (path.slice(0,1)=='/'){
		this.path='/'
		path=path.slice(1)
	}
	var myp=this.path.split('/')
	var paths=path.split('/')
	
	for (var i=0;i<paths.length;i++){
		if (paths[i]=='..')
			myp=myp.splice(0,myp.length-1)
		else
			myp.push(paths[i])
	}
	this.updateContents(myp.join('/'))
}
