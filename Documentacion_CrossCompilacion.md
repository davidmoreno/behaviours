Documentación - CrossCompilación de behaviours en RaspBerry Pi (con Raspbian)
=============================================================================

Paquetes necesarios en la RaspBerry:
 - python2.7-dev
 - libxml2-dev
 - liblua5.2-dev
 - libjson0-dev

Otros paquetes (para facilitar el proceso):
 - cmake
 - rsync
 - symlinks

También deberíamos instalar onion.

 Para empezar, tenemos que instalar, en la máquina donde vayamos a compilar, las herramientas de crosscompilación. Se consiguen siguiendo este tutorial, excepto el paso 3, que lo haremos un poco diferente: 

    http://www.kitware.com/blog/home/post/426

 Mientras se instalan las x-tools (que tardan un rato), podemos ir instalando los paquetes necesarios en la RaspBerry Pi, esto lo hacemos por comodidad, para no tener que manejar librerías para arquitectura ARM en una arquitectura distinta. 
 Una vez instaladas las librerías, vamos a hacer una limpieza de enlaces simbólicos, para evitar que un enlace absoluto apunte a donde no debe cuando movamos las librerías al ordenador donde compilaremos.

    #en la RaspBerry
    sudo symlinks -cr /

 Después ya podemos utilizar rsync para traer las librerías de la RaspBerry a nuestro ordenador:

    mkdir directorio_raspberry
    cd directorio_raspberry
    rsync -vrl usuarioRasPi@dominioRasPi:/lib .
    rsync -vrl usuarioRasPi@dominioRasPi:/usr .

A partir de ahora, tenemos parte del sistema de raspbian en una carpeta, y unas herramientas de crossCompilación en otra.

Para solucionar un error extraño que da, tenemos que modificar un archivo de cabecera del sistema. En la parte del sistema de raspbian que hemos traído, editamos el fichero "usr/include/features.h". En la línea 323, sustituimos

    #include <bits/predefs.h>
    /* wchar_t uses ISO 10646-1 (2nd ed., published 2000-09-15) / Unicode 3.1.  */
    #define __STDC_ISO_10646__        200009L

por

    #include <stdc-predef.h>

Ahora tendremos que modificar el archivo "Toolchain-RaspBerry.cmake" para poner las rutas correctas. En concreto, las variables tienen que contener:
	
	CMAKE_C_COMPILER => ruta absoluta al compilador de C, por ejemplo: /home/victor/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-gcc

	CMAKE_CXX_COMPILER => ruta absoluta al compilador de C++, por ejemplo: /home/victor/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++

	CMAKE_FIND_ROOT_PATH => ruta absoluta a la estructura de archivos copiada con rsync, por ejemplo: /home/victor/RaspBerryPi/rootfs_raspbian

Una vez tenemos esto, solo queda empezar a compilar. Para ello, en la carpeta raíz de behaviours, creamos una carpeta "build", y dentro de ella, lanzamos CMAKE:

    cmake -DARM=True -DCMAKE_TOOLCHAIN_FILE=[ruta absoluta al Toolchain] ..
    # si queremos compilar en modo debug, habría que añadir -DCMAKE_BUILD_TYPE=Debug
    make

Ahora ya tendremos los ejecutables para ARM, solo tenemos que pasarlos a la RaspBerry e instalar.
Para más comodidad al instalar, es recomendable mover el contenido de la carpeta build a una carpeta con la misma ruta, para poder llamar a "sudo make install" y que el makefile se encargue de instalarlo todo.