(function(){
  
  var WebService=extend(Action, {paramOptions: [ {type:String,text:'url',name:'url',default:"http://localhost:8081"}, {type:String,text:'name',name:'service',default:"webservice"} ]})
  
  WebService.prototype.activate=function(){
    var text=this.svggroup.find('text')
    text.attr('fill','#00ff00')
    // Here call to web service
    var iframe = document.getElementById("hiddenDownloader");
    iframe.src = this.getParams().url;
  }
  
  main.behaviour.nodeFactory.add('webservice',WebService)
}())

// (function(){
//   
//   var Dai=extend(Action, {paramOptions: [ {type:String,maxlength:'24',text:'server',name:'server',default:"localhost:8081"},
//						  {type:String,text:'url',name:'server',default:"localhost:8081"})
//   
//   WebService.prototype.activate=function(){
//     var text=this.svggroup.find('text')
//     text.attr('fill','#00ff00')
//     // Here call to web service
//     var iframe = document.getElementById("hiddenDownloader");
//     iframe.src = "../data/current.dia";
//   }
//   
//   main.behaviour.nodeFactory.add('dai',Dai)
// }())


