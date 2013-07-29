Documentación: Cómo crear un plugin
===================================

Introducción
------------

En behaviours, un nodo es cada uno de los elementos que podemos utilizar en la interfaz, que pueden interconectarse entre sí. Los nodos pueden ser Action o Event (o BlockingAction).
* Event → Inicio de la cadena de ejecución, puede conectarse a acciones, pero no puede recibir ninguna conexión.
* Action → Acción dentro de la cadena de ejecución, puede conectarse a otras acciones y recibir conexiones de acciones o eventos.
* BlockingAction → Subclase de Action, que bloquea la ejecución en el seervidor, y la continúa cuando se le da la señal desde la interfaz.

Las conexiones entre nodos pueden llevar una guarda, es decir, una condición que hace que se ejecute un nodo u otro. Si un nodo tiene varias conexiones sin guarda, se elige una aleatoriamente, si alguna de ellas no es vacía y se evalúa a cierto, se elige esa conexión.

Crear un nuevo plugin
---------------------

### Estructura de directorios ###

Para crear un plugin en behaviours, hay que seguir una estructura de directorios concreta:

En el directorio plugins creamos uno con el nombre que llevará nuestro plugin. En este directorio crearemos los archivos de código C++ y una carpeta static con tres subcarpetas:

* nodes: Contendrá ficheros xml con los parámetros de los nodos de nuestro plugin.
* js: Contendrá ficheros javascript que definirán el comportamiento en la parte cliente de nuestro plugin.
* img: Contendrá las imágenes que utilizarán los nodos a modo de icono. Por defecto, el nodo utilizará como icono una imagen en formato png con el mismo nombre del nodo.

![Estructura directorio](https://github.com/fawques/behaviours/raw/62b5115a883e2fc14c340696823a305458edaa6c/plugins/Interfaz/static/img/plugin1.png)

### XML ###

Los ficheros XML definen los datos necesarios para mostrar el nodo en la interfaz. Cada uno de ellos contiene el siguiente código:


    <node-description id="testbasic">
	    <name lang="en">Test Basic</name>
	    <description lang="en">Just shows a message on console</description>	
	    <type>action</type>
		<icon>test</icon>
		<js>ejemplo.js</js>
	</node-description>

Donde el parámetro id, dentro de la etiqueta node-description, será el identificador de nuestro nuevo plugin, necesitamos tenerlo localizado ya que en el resto del plugin utilizaremos este id para referenciarlo.
<name> indica el nombre que aparecerá en la interfaz, según el idioma seleccionado para el navegador.
En <type> debemos poner action o event según si el nodo es una acción o un evento.
Al final de <node-description> pueden aparecer etiquetas <js> que enlazarán a los archivos de javascript, que deben estar en la carpeta js.

### Javascript ###

Todos los nodos (tanto acciones como eventos) heredan de una misma clase Node, esta clase tiene una serie de funciones predefinidas, que pueden sobrescribirse en el nodo de nuestro plugin:

* configure
Esta función es la encargada de mostrar la interfaz de configuración que se lanza al crear un nuevo nodo. Por defecto se visualizan los parámetros , pero se podrá cambiar consiguiendo mostrar lo que queramos.
* acceptConfigure
Es la encargada de actualizar los parámetros de configuración que utilicemos en configure.
* activate
Cuando el nodo es activado por la interfaz, se realiza lo que se haya especificado en la función.
* deactivate
Cuando el nodo se haya desactivado se ejecutará el contenido de la función.
* setName
Se utiliza para cambiar el nombre del nodo, por defecto tendrá la posición en la lista de nodos que le haya tocado.
* paint
La función paint crea el aspecto visual del botón que se creará del plugin en la interfaz.
* update
* getParams
* realtime_update
* paramOptions
* params
* t
* remove

Si dejamos el fichero javascript en blanco, o no lo enlazamos desde el XML, al crear un nuevo nodo nos saldrá una ventana de configuración con únicamente tres botones, ya que no hemos puesto ningún parámetro.

![Ventana configuración predefinida](https://github.com/fawques/behaviours/raw/62b5115a883e2fc14c340696823a305458edaa6c/plugins/Interfaz/static/img/plugin2.png)  

Si queremos dar a nuestro nodo un comportamiento más personalizado, tendremos que escribir código javascript sobrescribiendo las funciones anteriores.
Por cada parámetro que queramos que tenga nuestro nodo debemos  colocarle los parámetros que consideremos oportunos. Si cogemos como referencia el plugin webservice, vemos que tiene dos parámetros de configuración. 

![Ventana configuración custom](https://github.com/fawques/behaviours/raw/62b5115a883e2fc14c340696823a305458edaa6c/plugins/Interfaz/static/img/plugin3.png)

En el archivo de javascript de webservice, el objeto está creado de la siguiente forma:

	var WebService=extend(Action, {paramOptions: [ {type:String,text:'url',name:'url',default:"http://localhost:8081"},
        {type:String,text:'name',name:'service',default:"webservice"} ]})

Donde paramOptions es una lista de los parámetros que queremos poner y cada parámetro tiene:

* Type: puede ser Array, Number, Text u otra cosa. Si es uno de los tipos predefinidos, el menú de configuración por defecto mostrará una representación concreta (si es Array, un desplegable; si es Number, un slider; y si es Text, un textarea). Si el tipo no es uno de los predefinidos, aparecerá un textbox sencillo.
* Text: nombre del parámetro que se mostrará en el menú de configuración.
* Default: el valor por defecto del parámetro.

Una vez establecidos los parámetros que tendrá nuestro nodo, podemos interactuar con ellos con las funciones anteriores. Para no utilizar la versión por defecto, necesitamos sobreescribirlas.

Un ejemplo :

	WebService.prototype.activate=function(){
    	var text=this.svggroup.find('text')
    	text.attr('fill','#00ff00')
	}

Donde webservice sobreescribe la función activate consiguiendo que nuestro Event se ilumine y cambie de color  en el momento en el que se activa el nodo.

### C++ ###

La sección de código escrita en C++ es la que se ejecutará en el servidor. Para ello, tendremos que crear una clase que represente nuestro nodo, con las funciones necesarias para su funcionamiento y enlazado con la interfaz.

Como ejemplo tenemos el nodo testBasic.
En el archivo .cpp podemos encontrar:

	void ab_init(void){
		DEBUG("Loaded ab init at basic example");
		AB::Factory::registerClass<Basic>("testbasic");
	}

	Basic::Basic(const char* type): Action(type)
	{
	}

	void Basic::exec()
	{	INFO("Ejecutando el nodo");
	}

Y en el .hpp encontramos la declaración de la clase y las funciones implementadas en el .cpp

	class Basic : public AB::Action{
	public:
		Basic(const char* type);	
		virtual void exec();
	};

La función ab_init se encarga de enlazar las clases de C++ con los nodos definidos en el XML. Así, la función AB::Factory::registerClass<Basic>("testbasic") enlaza un nodo del tipo Basic (en nuestro caso el nombre que le hayamos dado a nuestra clase), con el id del nodo en el xml. Solo debe aparecer una vez en todo el plugin.

En el constructor de la clase debemos llamar al constructor de la clase base (Action o Event), e inicializar los parámetros que pueda guardar el objeto.

Otra función importante es la función exec. Esta función se ejecutará cuando la cadena de ejecución en el servidor llegue al nodo. Si el nodo es de tipo BlockingAction, esta función debería llamar al método exec de la clase padre si queremos que la ejecución en el servidor se pare hasta que se le notifique que continúe. Por ejemplo:

	void Ejemplo::exec()
	{
	  INFO("En este punto, el servidor aún no se ha parado");
	  AB::BlockingAction::exec();
	  INFO("En este punto, ya se ha notificado al servidor que continúe");
	}

Si se quiere personalizar  el comportamiento de nuestro plugin, se pueden sobreescribir las funciones por defecto que utiliza de Node.cpp.
En un ejemplo más complejo, que utilice parámetros y sobrescriba las funciones:

	virtual AttrList attrList();
	virtual Object attr(const std::string& name);
	virtual void setAttr(const std::string& name, Object obj);

Por ejemplo, la función  attrList debe devolver una lista con los parámetros del nodo:

	AB::AttrList Ejemplo::attrList()
	{
	  AB::AttrList lista;
	  lista.push_back("param1");
	  lista.push_back("param2");
	  return lista;
	}

La función attr debe devolver el atributo que se haya almacenado en el nodo, según el nombre que se pida:

	AB::Object Ejemplo::attr(const std::string &k)
	{
	    if (k == "param1")
	    {
	   	 return to_object(this.param1);
	    }
	    else if (k == "param2")
	    {
	   	 return to_object(this.param2);
	    }
	}

La función setAttr  asigna un valor al  parámetro correspondiente. Para ello, se pasa como atributos k (nombre del parámetro) y s (valor que se le quiere dar).

	void Ejemplo::setAttr(const std::string &k, AB::Object s)
	{
	    if(k == "param1"){
	      this.param1=object2string(s);
	    }
	    else if (k == "param2")
	    {
	      this.param2=object2string(s);
	    }
	}

Compilación y ejecución
-----------------------

### CMakelist.txt ###

Para compilar el sistema se utiliza CMake.
En el archivo CMakeLists.txt de la carpeta del plugin se buscan las librerías y fuentes necesarios para la compilación del plugin, y se indica una ruta de instalación para los binarios y los componentes estáticos.
Un ejemplo de CMakeLists.txt puede ser:

	SET(SOURCES ejemplo.cpp otro_fuente_ejemplo.cpp)

	find_library(PYTHON2_LIB NAMES python2.7 PATH ${LIBPATH})
	find_path(PYTHON2_HEADER Python.h ${CMAKE_INCLUDE_PATH} usr/include/python2.7)

	add_library(Ejemplo SHARED ${SOURCES})
	target_link_libraries(Interfaz ${PYTHON2_LIB})
	include_directories(${PYTHON2_HEADER})
	link_directories(${PYTHON2_LIB})


	install(TARGETS Ejemplo
	    LIBRARY DESTINATION lib/ab
	    )
	    
	install(DIRECTORY static/nodes static/img static/js
	    DESTINATION shared/ab/static/Ejemplo
	    )

La instrucción SET(SOURCES ejemplo.cpp otro_fuente_ejemplo.cpp) guarda los ficheros de código C++ que utilizará el plugin.
Después se busca la librería de python y los archivos de cabecera de la misma.

Por último, se crea la librería que contendrá el plugin, además de añadir las opciones de enlazado para el compilador, y se indica las rutas de instalación de los binarios (en lib/ab) y de los tres elementos de la carpeta static (en shared/ab/static/Ejemplo).

### Órdenes de compilación y ejecución ###

Cada vez que cambiemos nuestro código debemos dirigirnos a la carpeta build de nuestro behaviours y ejecutar los siguientes comandos:

	Creamos la carpeta build en el directorio de behaviours: 
	$ mkdir build
	$cd build		
	$cmake -DCMAKE_INSTALL_PREFIX=/tmp/  ..
	$make 
	$sudo make install
	$abserver

Además refrescamos nuestro navegador para actualizar los cambios realizados en nuestro plugin.











