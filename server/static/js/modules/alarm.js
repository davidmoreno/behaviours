/**
 * @short Alarm is an event, triggered on a concrete date and time
 */

 (function(){
  
  var Alarm=extend(Event, {paramOptions: [{type:Array,values:[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31],name:'day'},
    {type:Array,values:[current_language.jan,current_language.feb,current_language.mar,current_language.apr,current_language.may,current_language.jun,current_language.jul,current_language.aug,current_language.sep,current_language.oct,current_language.nov,current_language.dec],name:'month'},
    {type:Array,values:[2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,2036,2037,2038,2039,2040,2041],name:'year'},
    {type:Array,values:['00','01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23'],name:'hour'},
    {type:Array,values:['00','01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59'],name:'minute'},
    {type:Array,text:current_language.repeat,values:[current_language.never,current_language.always,current_language.weekdays,current_language.weekend,current_language.mon,current_language.tue,current_language.wed,current_language.thu,current_language.fri,current_language.sat,current_language.sun],name:'repeat'},
    {type:Array,values:['YES','NO'],name:'nodeon'},
    {type:Array,values:['Never','01','02','03','04','05','06','07','08','09','10','Always'],name:'noderepeat'}
    ]})


Alarm.prototype.configure = function(){
  var that = this
  this.configureDialogSetup()
  
  //   try {
  //     var date = new Date($.ajax({'type': 'GET', 'url': '/'}).getResponseHeader('Date'));
  //   }
  //   catch(err) { 
  //     var date = null;
  //   }
  // 
  //   var timeBot=date
  //   var timeLocal = new Date()
  //   alert("Local machine time is: " + timeLocal + "<br> Server time is: " + timeBot)
  
  var p=this.paramOptions
  

  var ul=$('<ul>')
  
  var li=$('<li>')
  ul.append(li)
  li.text(current_language.date)
  for (var i=0;i<3;i++){  
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
   if(this.params && this.params['repeat']!=0)
     cb.attr('disabled',true)
   if(p[i].name == 'day')
     cb.val(Number(this.params[p[i].name])-1)
   else if (p[i].name == 'year')
     cb.val(Number(this.params[p[i].name])-113)
   else
     cb.val(this.params[p[i].name])
   li.append(cb)
 }
 
 var li=$('<li>')
 ul.append(li)
 li.text(current_language.time)
 for (var i=3;i<5;i++){  
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
}

var li=$('<li>')
ul.append(li)
li.text(current_language.repeat)
var i=5
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
li.text("Activate event")
var i=6
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
var i=7
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

Alarm.prototype.realtime_update = function(){
  this.params=this.getParams()
  for (var i=0;i<3;i++)
    $('#dialog #content #'+i).attr('disabled',false)
  
    // If user sets February, update 'day' array to 28 elements 
    if (this.params['month']==1) {
      this.paramOptions[0].values=[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28]
      this.configure();
      
    // If user sets April, June, September or November, update 'day' array to 30 elements
  } else if (this.params['month']==3 || this.params['month']==5 || this.params['month']==8 || this.params['month']==10) {
    this.paramOptions[0].values=[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30]
    this.configure()
    
    // Default 'day' array 31 elements
  } else {
    this.paramOptions[0].values=[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31]
    this.configure()
  }
}

Alarm.prototype.getParams = function(){
  var p=this.paramOptions
  if (p){
    var dialog=$('#dialog:visible')
    if (dialog.length==1){
      var params={}
      for(var i in p){
       var val=$('#dialog #content #'+i).val()

       if(p[i].name == 'day')
         params[p[i].name]=Number(val)+1
       else if(p[i].name == 'year')
         params[p[i].name]=Number(val)+113
       else
         params[p[i].name]=val
       
       if(this.id.indexOf(this.type)==-1){ 
        if(p[i].name=="nodeon"){
         if(this.changeactivity==true){
          val=this.changevalor
          params[p[i].name]=val 
          
        }
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
        if(!val){
          val=$("#"+this.id+" #noderepeat"+this.id).text()
        }
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


Alarm.prototype.update = function(){
  if (!this.params)
    return
  var txt=[]
  if (this.params['repeat'] == 0) {
    for (var i=0;i<5;i++) {
     if(this.paramOptions[i].name == 'day')
       txt.push(this.paramOptions[i].values[Number(this.params[this.paramOptions[i].name])-1])
     else if(this.paramOptions[i].name == 'year')
       txt.push(this.paramOptions[i].values[Number(this.params[this.paramOptions[i].name])-113])
     else
       txt.push(this.paramOptions[i].values[this.params[this.paramOptions[i].name]])
   } 
   txt.push(this.paramOptions[6].values[this.params[this.paramOptions[6].name]])
   txt.push(this.paramOptions[7].values[this.params[this.paramOptions[7].name]])

 } else {
  for (var i=3;i<8;i++) {
   txt.push(this.paramOptions[i].values[this.params[this.paramOptions[i].name]])
 }
}
if(this.id.indexOf(this.type)==-1){
  if(this.paramOptions[6].values[this.params[this.paramOptions[6].name]]=="NO"){
   

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
  if(this.paramOptions[7].name=="noderepeat"){
    if(this.paramOptions[7].values[this.params[this.paramOptions[7].name]]==11)                            
      $('text#noderepeat'+this.id).text("Always")              
    else
      $('text#noderepeat'+this.id).text(""+this.paramOptions[7].values[this.params[this.paramOptions[7].name]])
  }
}
$('text#noderepeat').text(""+this.paramOptions[7].values[this.params[this.paramOptions[7].name]])
txt=txt.join(' · ')
this.width=txt.length*25
$('#'+this.id+' text#param').text(txt)
$('#'+this.id+' rect#node').attr('width',this.width)
}

Alarm.prototype.acceptConfigure=function(){
 try{
  this.params=this.getParams()
  this.update()
  alert(current_language.alert_bot_time);
}
catch(e){
  alert(current_language.alert_accept_configure);
}
}
main.behaviour.nodeFactory.add('alarm',Alarm)
}())