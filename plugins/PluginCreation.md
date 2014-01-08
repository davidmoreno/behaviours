Documentation: Creating plugins
===================================

Introduction
------------

In behaviours, a node is every element that can be used in the interface and can be connected between each other. Nodes can be Events or Actions (or BlockingActions)
* Event → Start of the execution chain, it can be connected to actions, but it cannot receive any connection.
* Action → Action in the execution chain, it can be connected to other actions and receive connections from actions or events.
* BlockingAction → Subclass of Action. It blocks execution in the server, and it resumes it when a signal is sent from the interface.

Connections between nodes can carry a guard. Guards are simple LUA expressions that control if the chain execution must continue using that path. If the guard does not comply this path will not be taken. If there are several paths that comply with the guard, a random one will be taken.


Creating a new plugin
---------------------

### Directory tree ###

In order to create a plugin in behaviours, you have to follow a given directory structure:

Inside the `plugins` directory, create another directory named as the plugin. In this directory, create the C++ files and a `static` folder with three subfolders:

* nodes: Contains xml files with the plugin's nodes' parameters.
* js: Contains javascript files that define the plugin's behaviour in the client side
* img: Contains the images that nodes will use as icons. By default, a node will use as icon an image in png format with the same name as the node.

![Directory tree](https://github.com/fawques/behaviours/raw/62b5115a883e2fc14c340696823a305458edaa6c/plugins/Interfaz/static/img/plugin1.png)

### XML ###

XML files define the data needed to show nodes in the interface. Each of them contains the following code:

    <node-description id="testbasic">
	    <name lang="en">Test Basic</name>
	    <description lang="en">Just shows a message on console</description>	
	    <type>action</type>
		<icon>test</icon>
		<js>example.js</js>
	</node-description>

Where the parameter `id`, inside the tag `node-description`, is the identifier of the node. This id is important because it will be used to reference the node throughout the plugin.

`<name>` indicates the name which will appear in the interface, according to the language selected at the navigator.

`<type>` must contain `action` or `event` according to the node type.

At the end of `<node-description>`, `<js>` tags may appear. They will link to the javascript files in the `js` folder.

### Javascript ###

All nodes (both actions and events) inherit from a same class Node. This class has several predefined functions, which can be overwritten in the plugin's nodes:

* `configure`
This function is in charge of showing the configuration dialog launched when we create a new node. By default it lets you edit the parameters, but it can show whatever we want.
* `acceptConfigure`
This function is in charge of setting the configuration parameters modified in configure.
* `activate`
When the node is activated by the interface, this function is executed.
* `deactivate`
When the node is deactivated, this function is executed.
* `setName`
This function changes the node's name, the name by default is `node_` followed by its node number.
* `paint`
This function creates the visual look of the node in the interface.
* `update`
* `getParams`
* `realtime_update`
* `paramOptions`
* `params`
* `t`
* `remove`

If we leave the javascript file blank, or we don't link it from the XML file, when we create a new node a configuration dialog will be shown with only three buttons, since we haven't introduced any parameter.

![Default configuration dialog](https://github.com/fawques/behaviours/raw/62b5115a883e2fc14c340696823a305458edaa6c/plugins/Interfaz/static/img/plugin2.png)  

If we want to give the node a more personalized behaviour, we will have to write javascript code overwriting the previous functions.

For each parameter we want our node to have, we must write it when creating the javascript object. If we take as a reference the webservice plugin, we can see that it has two configuration parameters.

![Custom configuration dialog](https://github.com/fawques/behaviours/raw/62b5115a883e2fc14c340696823a305458edaa6c/plugins/Interfaz/static/img/plugin3.png)

In the webservice javascript code, the object is created as follows:

	var WebService=extend(Action, {paramOptions: [ {type:String,text:'url',name:'url',default:"http://localhost:8081"},
        {type:String,text:'name',name:'service',default:"webservice"} ]})

Where paramOptions is an array of parameters, and each parameter contains at least:

* `type`: can be Array, Number, Text or any other thing. If it is one of the predefined types, the default configuration menu will show a predefined look (if it is an Array, a dropdown; if it is a Number, a slider; and if it is a Text, a textarea). If the type is not one of the predefined ones, it will show a simple textbox.
* `text`: name of the parameter that will be shown in the configuration menu.
* `default`: default value of the parameter.

Once the node parameters are set, we can interact with them using the former functions. If we do not want the default behaviour of these functions, we need to overwrite them, for example:

	WebService.prototype.activate=function(){
    	var text=this.svggroup.find('text')
    	text.attr('fill','#00ff00')
	}

Webservice overwrites the `activate` function allowing the node to light up and change its color when it is activated.

### C++ ###

The code written in C++ is the one that will be executed in the server. We will have to create a class representing our node, with the functions needed for it to work and linked with the interface.

For instance, the testBasic node:
In the .cpp file we can find:

	void ab_init(void){
		DEBUG("Loaded ab init at basic example");
		AB::Factory::registerClass<Basic>("testbasic");
	}

	Basic::Basic(const char* type): Action(type)
	{
	}

	void Basic::exec()
	{	INFO("Executing the node");
	}

And in the .hpp file we find the class and functions declaration.

	class Basic : public AB::Action{
	public:
		Basic(const char* type);	
		virtual void exec();
	};

`ab_init` links C++ classes with the nodes defined in XML. Thus, the function AB::Factory::registerClass<Basic>("testbasic") links a node of type Basic (in our case, the name we have given to our class) with the node id in the xml file. `ab_init` can only appear once in the whole plugin.

The class constructor must call the base class constructor (Action or Event), and initialize the parameters that the object can store.

Another importatn function is the `exec` function. This function will be executed when the execution chain in the server reaches the node. If the node type is BlockingAction, this function should call the exec method from the parent class if we want the execution in the server to stop until signaled. For example:

	void Example::exec()
	{
	  INFO("At this point, the server is not stopped yet");
	  AB::BlockingAction::exec();
	  INFO("At this point, the server has been signaled and has resumed");
	}

If you want to customize the plugin's behaviour, you can overwrite the default functions used from Node.cpp
In a more complex example, using parameters and overwriting the functions:

	virtual AttrList attrList();
	virtual Object attr(const std::string& name);
	virtual void setAttr(const std::string& name, Object obj);

For example, the function attrList must return a list with the node parameters:

	AB::AttrList Example::attrList()
	{
	  AB::AttrList list;
	  list.push_back("param1");
	  list.push_back("param2");
	  return list;
	}

The function attr must return the attribute stored in the node, according to the name asked:

	AB::Object Example::attr(const std::string &k)
	{
	    if (k == "param1")
	    {
	   	 return to_object(this.param1);
	    }
	    else if (k == "param2")
	    {
	   	 return to_object(this.param2);
	    }
	}

The function setAttr assigns a value to the corresponding attribute. The function parameters are k (name of the attribute) and s (value you want to assign).

	void Example::setAttr(const std::string &k, AB::Object s)
	{
	    if(k == "param1"){
	      this.param1=object2string(s);
	    }
	    else if (k == "param2")
	    {
	      this.param2=object2string(s);
	    }
	}

Compilation and execution
-----------------------

### CMakelist.txt ###

In the CMakeLists.txt file in the plugin folder we look for the libraries and sources needed to compile the plugin, and we set an install path for binaries and static components. For example:

	SET(SOURCES example.cpp another_source_example.cpp)

	find_library(PYTHON2_LIB NAMES python2.7 PATH ${LIBPATH})
	find_path(PYTHON2_HEADER Python.h ${CMAKE_INCLUDE_PATH} usr/include/python2.7)

	add_library(Example SHARED ${SOURCES})
	target_link_libraries(Example ${PYTHON2_LIB})
	include_directories(${PYTHON2_HEADER})
	link_directories(${PYTHON2_LIB})


	install(TARGETS Example
	    LIBRARY DESTINATION lib/ab
	    )
	    
	install(DIRECTORY static/nodes static/img static/js
	    DESTINATION shared/ab/static/Example
	    )

The instruction `SET(SOURCES example.cpp another_source_example.cpp)` stores the C++ code files that the plugin will use. After that we look for the python library and its headers.

At last, we create a library that will contain the plugin, besides adding the linking options for compiling and the install paths of binaries (in lib/ab) and the three elements in the static folder (in shared/ab/static/example).

### Compilation and execution orders ###

Every time we change our code, we should compile it from the `build` folder in behaviours (we create it if it does not exist) and execute the following commands:

	# if we want it to be installed in a place different than the default, we add the -DCMAKE_INSTALL_PREFIX option
	$cmake -DCMAKE_INSTALL_PREFIX=/tmp/  ..
	$make 
	$sudo make install

After that, executing `abserver` and refreshing our browser will show our plugin modifications.











