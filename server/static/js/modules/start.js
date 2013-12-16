(function(){
  var Start=extend(Event,  {paramOptions: [{type:Array,values:['YES','NO'],name:'nodeon'},
   {type:Array,values:['Never','01','02','03','04','05','06','07','08','09','10','Always'],name:'noderepeat'}
   ]})
  Start.prototype.configure = function(){
   var that = this
   this.configureDialogSetup()
   var p=this.paramOptions
   var ul=$('<ul>')
   var li=$('<li>')
   ul.append(li)
   li.text("Activate event")
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
  li.text("Times repeat event")
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
    var dialog=$('#dialog:visible')
    if (dialog.length==1){
      var params={}
      for(var i in p){
        var val=$('#dialog #content #'+i).val()
        params[p[i].name]=val	
        if(this.id.indexOf(this.type)==-1){ 
          if(p[i].name=="nodeon"){

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
            if(val==11)                            
              $('text#noderepeat'+this.id).text("Always")              
            else
              $('text#noderepeat'+this.id).text(""+val)
          }
        }
      }
      return params;
    }
    return this.params
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

this.width=this.height
txt=txt.join(' · ')
$('#'+this.id+' text#param').remove()
$('#'+this.id+' rect').attr('width',this.width)
}
Start.prototype.acceptConfigure=function(){
  try{
    this.params=this.getParams()
    this.update()

  }
  catch(e){
    alert(current_language.alert_accept_configure);
  }
}
main.behaviour.nodeFactory.add('start',Start)
}())
