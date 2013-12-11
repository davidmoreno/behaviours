/**
 * Behaviours - UML-like graphic programming language
 * Copyright (C) 2013 Coralbits SL & AISoy Robotics.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <assert.h>
#include <string.h>
#include <fstream>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <sys/stat.h>

#include "event.h"
#include "action.h"
#include "manager.h"
#include "factory.h"
#include "connection.h"
#include "log.h"

using namespace AB;

static void readNodes(xmlNode *node, Manager *manager);
static void readNode(xmlNode *node, Manager *manager);
static void readConnection(xmlNode *action, Manager *manager);
static void readMetadata(xmlNode *meta, Manager *manager);
static void readFile(xmlNode *file);

std::string base64_decode(std::string const& encoded_string);

static inline bool is_base64(unsigned char c)
{
  return (isalnum(c) || (c == '+') || (c == '/'));
};

static const std::string base64_chars =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/";

/**
 * @short Do the reading of an XML file and inserts the data on the manager.
 */
void readBehaviour(const std::string &file, Manager *manager)
{
	{
		struct stat st;
		if (stat(file.c_str(), &st)<0){
			ERROR("Could not load behaviour from %s", file.c_str());
			return;
		}
	}
	
  LIBXML_TEST_VERSION;

  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;

  doc=xmlReadFile(file.c_str(), NULL, 0);

  root_element=xmlDocGetRootElement(doc);
  if (!root_element) {
    ERROR("Error reading behavior description file %s", file.c_str());
    return;
  }

  readNodes(root_element->children, manager);

  xmlFreeDoc(doc);
  xmlCleanupParser();
}

static void readNodes(xmlNode *node, Manager *manager)
{
  xmlNode *cur;
  for(cur=node; cur; cur=cur->next) {
    if (cur->type==XML_ELEMENT_NODE) {
			std::string nname{ (const char*)cur->name };
      if (nname=="node" || nname=="action" || nname=="event") {
        readNode(cur, manager);
      } else if (nname == "meta") {
        readMetadata(cur, manager);
      } else if (nname == "file") {
        readFile(cur);
      }
    }
  }
  // second round, connections
  for(cur=node; cur; cur=cur->next) {
    if (cur->type==XML_ELEMENT_NODE) {
      if (strcmp((char*)cur->name, "connection")==0) {
        readConnection(cur, manager);
      }
    }
  }

}


static void readNode(xmlNode *node, Manager *manager)
{
  xmlChar *p=xmlGetProp(node, (const xmlChar*)"type");
  if (!p)
    return;
  Node::p ev;
  try {
    ev=Factory::createNode((const char*)p);
  } catch(const Factory::type_does_not_exists &e) {
    WARNING("Could not create event: %s", e.what());
    WARNING("Known types: %s", Factory::list_as_string().c_str());
    xmlFree(p);
    return;
  }
  xmlFree(p);
	assert(bool(ev));
	
  p=xmlGetProp(node,(const xmlChar*)"id");
  if (p) {
    ev->setName((char *)p);
    xmlFree(p);
  }
  
  AB::Point2D pos;
  p=xmlGetProp(node,(const xmlChar*)"x");
  if (p) {
    pos.x = atof((char *)p);
    xmlFree(p);
    p=xmlGetProp(node,(const xmlChar*)"y");
    if (p) {
      pos.y = atof((char *)p);
      ev->setPosition(pos);
      xmlFree(p);
    } 
  }
  
  

  xmlNode *param;
  for(param=node->children; param; param=param->next) {
    if (strcmp((char*)param->name, "param")==0 && param->children) {
      p=xmlGetProp(param, (const xmlChar*)"name");
      if (p) {
        ev->setAttr((const char*)p, (const char*)param->children->content);
        xmlFree(p);
      } else {
        ev->setAttr("", (const char*)param->children->content);
      }
    }
  }
  manager->addNode(ev);
}

/**
 * @short Parses the guard
 *
 * Gets the [] part of a string and returns it
 */
static std::string parseGuard(const std::string &s)
{
  std::string::const_iterator I, endI;
  bool atB=false;
  std::string ret;
  for(I=s.begin(),endI=s.end(); I!=endI; ++I) {
    if (atB) {
      if (*I==']') {
        return ret;
      }
      ret+=*I;
    }
    if (*I=='[') {
      atB=true;
    }
  }
  return ret;
}

static void readConnection(xmlNode *node, Manager *manager)
{
  xmlChar *id=xmlGetProp(node,(const xmlChar*)"id");
  xmlChar *from=xmlGetProp(node,(const xmlChar*)"from");
  xmlChar *to=xmlGetProp(node,(const xmlChar*)"to");
  if (!id || !from || !to)
    return;

  Connection::p conn=manager->connect((const char*)from, (const char*)to);

  /// Could not create connection
  if (!conn) {
    WARNING("Could not create connection %s -> %s. Most probable cause is objects do not exist.", from, to);
    return;
  }

  if (node->children) {
    xmlNode *child;
    for(child=node->children; child; child=child->next) {
      if (strcmp((char*)child->name, "guard")==0 && child->children) {
        conn->setGuard(parseGuard((const char*)child->children->content));
      }
    }
  }

  xmlFree(id);
  xmlFree(from);
  xmlFree(to);
}

/**
 * @short Parses current user behaviour name, description and viewpoint 
 *
 */
static void readMetadata(xmlNode *node, Manager *manager)
{
  xmlNode *param;
  for(param=node->children; param; param=param->next) {
    if ( strcmp((char*)param->name, "name") == 0 && param->children ) {
        manager->setName((const char*)param->children->content);
        DEBUG("Setting name to %s", (const char*)param->children->content);
    } else if (strcmp((char*)param->name, "description")==0 && param->children) {
	manager->setDescription((const char*)param->children->content);
	DEBUG("Setting description to %s", (const char*)param->children->content);
    } else if (strcmp((char*)param->name, "viewpoint")==0 && param->children) {
	manager->setViewpoint((const char*)param->children->content);
    }
  }
}

/**
 * @short Saves encoded files
 *
 * Decodes base64 encoded files and saves them with the proper file name in 'data/'
 */
static void readFile(xmlNode *node)
{
  
  xmlChar *p=xmlGetProp(node, (const xmlChar*)"id");
  if (!p)
    return;
  std::string path("data/files/audio/");
  std::string filename = path+(const char*)p;
  xmlFree(p);

  if(!node->children)
    return;
  mkdir(path.c_str(),0777);
  
  std::string data = (const char*)node->children->content;
  //INFO(data.c_str());
  std::string decoded = base64_decode(data);
 
  //INFO(decoded.c_str());
  std::ofstream f(filename.c_str(), std::ios::binary);
  f<< decoded;
  f.close();
}

std::string base64_decode(std::string const& encoded_string)
{
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}