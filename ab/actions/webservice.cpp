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

#include <ab/log.h>

#include <curl/curl.h>
#include <curl/easy.h>

#include "webservice.h"
#include <ab/manager.h>

using namespace AB;

int webservice_writer(char *data, size_t size, size_t nmemb, std::string *buffer) {
  buffer->append(data);
  return 0;
}

WebService::WebService(const char *type) : Action(type)
{
  service = "webservice";
  url="http://localhost:8081";
  timeout = 1000;
}

void WebService::setAttr(const std::string &k,AB::Object t)
{
  if (k=="service") service=object2string(t);
  else if (k=="url") url=object2string(t);
  else if (k=="timeout") timeout=object2int(t);
  else Action::setAttr(k,t);
}

AB::Object WebService::attr(const std::string &k)
{
  if (k=="service") return to_object(service);
  else if (k=="url") return to_object(url);
  else if (k=="timeout") return to_object(timeout);
  else return Action::attr(k);
}

AttrList WebService::attrList()
{
  AttrList l=Action::attrList();
  l.push_back("service");
  l.push_back("url");
  l.push_back("timeout");
  return l;
}

void WebService::exec()
{
  INFO("service: %s, url: %s, timeout: %d", service.c_str(), url.c_str(), timeout);
  CURL *curl;
  std::string response="";

  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_NOBODY, true);
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);
  curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, true);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, webservice_writer);

  CURLcode code = curl_easy_perform(curl);

  curl_easy_cleanup(curl);

  DEBUG("Web request got: %d", code);
}

void WebService::setManager(AB::Manager* m)
{
  manager=m;
}
