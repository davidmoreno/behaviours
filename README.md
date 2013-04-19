Compilation instructions
------------------------

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make 
    $ make install
    
Execute
-------

    $ abserver

Point your browser to http://localhost:8081

Install to custom path
----------------------

By default it installs to /usr/share, but can be changed to for example /tmp/ to ease development.

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX=/tmp/ ..
    $ make 
    $ make install

Also available is -DCMAKE_BUILD_TYPE=Debug, which compiles AB in debug mode, which shows 
more information as it executes.

Creating plugins
----------------

Check plugins directory for some examples. basic is a really basic plugin that creates a new action.

Basically you must create a .so file with a `void ab_init(void)` function. It can register new node 
types and so on. As new requisites appear the signature may change and more facilities given.

Also user can create a subdirectory at `${ABPATH}/shared/ab/static/[yourname]/` to add static content. It has a 
reverse alphabetical order priority, which means that is a file exists in both `aa` and `bb`, the one in
`bb` will be returned. This allows to create custom skins, create new node definitions and generally
expand the HTML5 interface.

License
-------

Behaviour is AGPLv3 licensed.
