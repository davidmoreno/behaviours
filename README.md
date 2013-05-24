Behaviours is a platform to create complex programs and behaviours just connecting the simple actions t
hat conform a program. 

Normally the program is based on several simple algorithms and behaviours and there is a lot of plumbing
between the elements. With behaviours the plumbing is simplified to the maximum just connecting boxes, 
letting the programmer concentrate on high level concepts.

This allows to create complex programs with just some drag and drop.

Plase check the [wiki](https://github.com/davidmoreno/behaviours/wiki) for more information on how to use
behaviours and how to expand it.

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


License
-------

Behaviour is AGPLv3 licensed.

Anybody can use it, copy it, share it under the same terms. Code modifications must be shared with your users.

This is a Free Software project; any help is appreciated, be it code, documentation, helping awareness of 
the program... or anything you may think of. We are eager to hear from people interested in Behaviours.
