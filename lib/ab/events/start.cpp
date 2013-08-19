#include <sstream>

#include <ab/log.h>

#include <ab/manager.h>

#include "start.h"
using namespace AB;
using namespace std;

void Start::setAttr(const std::string &k, Object s){
 			if(k== "nodeon"){
 				nodeon = object2int(s);  
 				printf("%d\n",nodeon );
 				if(nodeon==0){
 					
 					if(manager){
 						WARNING("Va a introducir el evento");        
 						if(!manager->findNode(this->name())){
 							WARNING("Mete el evento");
 							manager->addEvent(event);
 						}
 					}
 				}
 				else{
 					if(manager){
 						if(manager->findNode(this->name())){
 							WARNING("Borra el evento");
 							event=manager->getEvent(this->name());
 							manager->removeEvent(this->name());
 						}
 					}
 				}
 				      
 				DEBUG("start nodeon requested: %d", nodeon);
 				return;
 			}
 			else if(k== "noderepeat"){
 				noderepeat = object2int(s);
 				DEBUG("start noderepeat requested: %d", noderepeat);
 				return;
 			}
 			Event::setAttr(k,s);

}