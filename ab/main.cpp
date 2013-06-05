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

#include <ab/manager.h>
#include <memory>
#include <signal.h>
#include <ab/builtin_nodes.hpp>

std::shared_ptr<AB::Manager> manager;

static void on_SIGINT(int){
	static bool twice=false;
	if (twice){
		fprintf(stderr, "Exiting on second SIGINT.");
		exit(1);
	}
	twice=true;
	if (manager.get())
		manager->cancel();
}

int main(int argc, char **argv){
	if (argc!=2){
		fprintf(stderr,"ab <abfile>\n");
		return 1;
	}
	
	signal(SIGINT, on_SIGINT);
	signal(SIGTERM, on_SIGINT);

	manager=std::make_shared<AB::Manager>();
  AB::registerBuiltinNodes();
	
	
	manager->loadBehaviour(argv[1]);
	manager->exec();
	return 0;
}
