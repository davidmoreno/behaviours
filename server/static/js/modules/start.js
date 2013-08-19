(function(){
var Start=extend(Event,  {paramOptions: [{type:Array,values:['YES','NO'],name:'nodeon'},
             {type:Array,values:['00','01','02'],name:'noderepeat'}
            ]})
Start.prototype.configure = function(){
	var that = this
    this.configureDialogSetup()
    var p=this.paramOptions
    var ul=$('<ul>')
    var li=$('<li>')
    ul.append(li)
    li.text(current_language.repeat)
    var i=0
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

    $('#dialog #title').html(current_language.configuration_of+this.type+' <span class="name">(object id '+this.id+')</span>')
    $('#dialog #content').html(ul)
}

Start.prototype.realtime_update = function(){
    this.params=this.getParams()      
  }

Start.prototype.getParams = function(){
	var p=this.paramOptions
    if (p){
      var params={}
      for(var i in p){
		var val=$('#dialog #content #'+i).val()
			params[p[i].name]=val	
		    if(this.id.indexOf(this.type)==-1){ 
		      if(p[i].name=="nodeon"){

		        if(val==0){

		          $('image#nodeonoff'+this.id).attr('href','img/on.png')
		        }
		        else{
		          $('image#nodeonoff'+this.id).attr('href','img/off.png')
		        }

		      }
		      if(p[i].name=="noderepeat"){

		        $('text#noderepeat').text(""+val)
		      }
		    }
    	}
    	return params;
    }
    return {}

}
Start.prototype.update=function(){
	if (!this.params)
    	return
    var txt=[]
  	for(var i=0;i<2;i++){
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
    
	this.width=this.height
	txt=txt.join(' · ')
	$('#'+this.id+' text#param').remove()
	$('#'+this.id+' rect').attr('width',this.width)
}
  Start.prototype.acceptConfigure=function(){
    try{
    this.params=this.getParams()
    this.update()
    alert(current_language.alert_bot_time);
    }
    catch(e){
      alert(current_language.alert_accept_configure);
    }
  }
main.behaviour.nodeFactory.add('start',Start)
}())
