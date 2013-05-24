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

#include <string.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <sys/stat.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "event.h"
#include "action.h"
#include "manager.h"
#include "factory.h"
#include "connection.h"
#include "log.h"


using namespace AB;

static void writeNodes(xmlNode *node, Manager *manager, bool includeFiles = false);
static void writeMetadata(xmlNode *meta, Manager *manager);
static std::string attachFile(std::string const& filename);
static void sanitizeXmlString(std::string &value);

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);

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
void writeBehaviour(const std::string &file, Manager *manager, bool includeFiles)
{
  LIBXML_TEST_VERSION;

  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;

  doc = xmlNewDoc(BAD_CAST "1.0");
  root_element = xmlNewNode(NULL, BAD_CAST "ab");
  xmlDocSetRootElement(doc, root_element);

  writeNodes(root_element, manager, includeFiles);
  writeMetadata(root_element, manager);
  int n=xmlSaveFormatFileEnc(file.c_str(), doc, "UTF-8", 1);
	if (n<0){
		ERROR("Error writing XML file: %s", strerror(errno));
	}
  
  xmlFreeDoc(doc);
  xmlCleanupParser();
}

static void writeNodes(xmlNode *root_element, Manager *manager, bool includeFiles)
{
  xmlNode *cur;
  Action *a;
  Event *ev;
  
  for(Node *node: manager->getNodes()) {
    if((a = dynamic_cast<Action*>(node)) != NULL) {
      
      cur = xmlNewChild(root_element, NULL, BAD_CAST "action", NULL);
      xmlNewProp(cur, BAD_CAST "id", BAD_CAST a->name().c_str());
      xmlNewProp(cur, BAD_CAST "type", BAD_CAST a->type());
      
      char x[32];
      char y[32];
      sprintf(x,"%lf",a->position().x);
      sprintf(y,"%lf",a->position().y);

      xmlNewProp(cur, BAD_CAST "x", BAD_CAST x);
      xmlNewProp(cur, BAD_CAST "y", BAD_CAST y);
      if(std::string(a->type()) != "imagecapture") {
	for(std::string key: node->attrList()) {
	  std::string value = object2string(node->attr(key));
	  sanitizeXmlString(value);
	  WARNING("%s: %s",key.c_str(), value.c_str());
	  xmlNode *param = xmlNewChild(cur, NULL, BAD_CAST "param", BAD_CAST value.c_str());
	  WARNING("Got param");
	  if(param)
	    xmlNewProp(param, BAD_CAST "name", BAD_CAST key.c_str());
	  else WARNING("Did not get param");

	  if(key == "filename" && strcmp(a->type(),"audio") && includeFiles) {
	    std::string file_ = attachFile(std::string("data/files/audio/")+value);
	    xmlNode *f = xmlNewChild(root_element, NULL, BAD_CAST "file", BAD_CAST file_.c_str());
	    xmlNewProp(f, BAD_CAST "id", BAD_CAST value.c_str());
	  }
	}
      }
    } else if ((ev = dynamic_cast<Event*>(node)) != NULL && ev->name().substr(0,2) != "__" ) {
      cur = xmlNewChild(root_element, NULL, BAD_CAST "event", NULL);
      xmlNewProp(cur, BAD_CAST "id", BAD_CAST ev->name().c_str());
      xmlNewProp(cur, BAD_CAST "type", BAD_CAST ev->type());
      
      char x[32];
      char y[32];
      sprintf(x,"%lf",ev->position().x);
      sprintf(y,"%lf",ev->position().y);

      xmlNewProp(cur, BAD_CAST "x", BAD_CAST x);
      xmlNewProp(cur, BAD_CAST "y", BAD_CAST y);
      for(std::string key: node->attrList()) {
	  std::string value = object2string(node->attr(key));
	  sanitizeXmlString(value);
	  WARNING("%s: %s",key.c_str(), value.c_str());
	  xmlNode *param = xmlNewChild(cur, NULL, BAD_CAST "param", BAD_CAST value.c_str());
	  WARNING("Got param");
	  if(param)
	    xmlNewProp(param, BAD_CAST "name", BAD_CAST key.c_str());
	  else WARNING("Did not get param");
	  if(key == "filename" && includeFiles) {
	    std::string file_ = attachFile(value);
	    xmlNode *f = xmlNewChild(root_element, NULL, BAD_CAST "file", BAD_CAST file_.c_str());
	    xmlNewProp(f, BAD_CAST "id", BAD_CAST value.c_str());
	  }
      }  
    } else {
      WARNING("Node %s, is neither an Action nor an Event",node->name().c_str()) 
    }
      
  }
  // second round, connections
  for(Connection *connection: manager->getConnections()) {
    cur = xmlNewChild(root_element, NULL, BAD_CAST "connection", NULL);
    xmlNewProp(cur, BAD_CAST "id", BAD_CAST connection->name().c_str());
    xmlNewProp(cur, BAD_CAST "from", BAD_CAST connection->from()->name().c_str());
    xmlNewProp(cur, BAD_CAST "to", BAD_CAST connection->to()->name().c_str());
    if (!connection->guard().empty()) {
      std::string _g = std::string("[")+connection->guard()+"]";
      xmlNewChild(cur, NULL, BAD_CAST "guard", BAD_CAST _g.c_str());
    }
  }

}

/**
 * @short Parses current user given name, description and viewpoint 
 *
 */
static void writeMetadata(xmlNode *root_element, Manager *manager)
{
  xmlNode *cur = xmlNewChild(root_element, NULL, BAD_CAST "meta", NULL);
  if(manager->viewpoint().size() == 6) {
    std::ostringstream viewpoint;
    viewpoint << "matrix(" << std::setprecision(20) << manager->viewpoint()[0] << "," << manager->viewpoint()[1] << "," << manager->viewpoint()[2] << "," << manager->viewpoint()[3] << "," << manager->viewpoint()[4] << "," << manager->viewpoint()[5] << ")";

    xmlNewChild(cur, NULL, BAD_CAST "viewpoint", BAD_CAST viewpoint.str().c_str());
    
  } else {
    xmlNewChild(cur, NULL, BAD_CAST "viewpoint", BAD_CAST "matrix(1,0,0,1,0,0)");
  }
  xmlNewChild(cur, NULL, BAD_CAST "name", BAD_CAST manager->name().c_str());
  xmlNewChild(cur, NULL, BAD_CAST "description", BAD_CAST manager->description().c_str());
}

/**
 * @short Attaches encoded files
 *
 * Encodes files into base64 and attach them to the XML file
 */
static std::string attachFile(const std::string & filename)
{
  struct stat filestat;

  std::string encoded = "";
  
  if (stat(filename.c_str(), &filestat)) {
    WARNING("Unable to encode %s: file not found",filename.c_str());
    return encoded;
  } 
  char * data = new char[filestat.st_size];
  std::ifstream f (filename.c_str(), std::ios::in | std::ios::binary);
  f.read(data, filestat.st_size);
  encoded = base64_encode((unsigned char*)data,filestat.st_size);
  //WARNING(encoded.c_str());
  delete [] data;
  f.close();
  return encoded;
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;
}

static void sanitizeXmlString(std::string & value) {
  size_t pos = 0;
  while ((pos = value.find_first_of("&\"\'¿¡<>|",pos)) != std::string::npos) {
      std::string subs;
      switch(value[pos]) {
        case '&': subs = "amp;"; break;
        case '<': subs = "lt;"; break;
        case '>': subs = "gt;"; break;
        case '\"': subs = "quot;"; break;
        case '\'': subs = "apos;"; break;
// FIXME: those are two characters, so find out which one is first and remember to substitute both, so treat them separatedly
//        case '¿': subs = "iquest;"; break;
//        case '¡': subs = "iexcl;"; break;
        case '|': subs = "brvbar;"; break;
      }
      value.replace(pos,1,"&");
      value.insert(++pos,subs);
  }
}

