Coronis Test
============

Coronis Test started as a need to fix the frustration of use Selenium Core and it lack of AJAX and Asynchronous support. It can be done, but it is a pain to do it.

With Coronis Test you write the tests and there is a time lapse that can take to be true. It does not need to be inmediatly, it can take several seconds, the time it gets to get the data from server, and still the test will pass. And the same with "negatives".

Also with Selenium it was not possible to easily create "subtasks", tasks that are repeated a lot in the program, like login, logout, create an user... All this can be included in Coronis Test with no problem, using subtasks. The same applies to add custom javascript, so you can create your custom commands as subtasks and using javascript.

Finally at selenium all processing was quite linear. In Coronis Test they are linear too, but it allows at least to include external files, so you can have a file with your tests subdivided, or with the common functions.

It uses jQuery, but until now I have not found any problem with any other Javascript framework, as the pages run isolated in an iframe. Also using jQuery is instrumental to make the core code of Coronis Test small.


Use it
======

To use it you need to use it thought a web server, launch the ctest.html file, and in there select you suite.ctest which should `include(...)` your tests, or do the test straight away.


Syntax
======

Each ctest is composed of several commands, one per line, function definitions, or embedded javascript. Default commands are listed at https://github.com/davidmoreno/coronistest/blob/master/COMMANDS.txt .

All syntax resembles slightly to javascript.

It understands constant strings and variable strings. Simple assign is made with the concat('var',...) argument.

Whenever an element must be defined, it is using jQuery select syntax: #id, @class, el...


Functions
---------

Functions are defined using javascript like syntax:

  login = function(user, password){
    click('link=Login');
    type('#username',user)
    type('#password',password)
    click('input[type=submit]')
    errorOnNext()
    checkText('Error on login')
    errorOnNext()
    checkElement('link=Login')
  }

  login('admin','admin')

Inline javascript
-----------------

It can embed any javascript code inside the test. It acts as a single command, and it will be executed when
at that line. It is useful for example to define new custom commands that need javascript. This can be performed 
as:

  javascript{
	commands['fail'] = function(){
		throw { text:"Always fails", may_appear_later:false }
	}
  } 

On this example we create a custom javascript command that always fail.


More on creating custom commands
--------------------------------

Just now there is not a lot of documentation, but please, check the commands.js file to check how the custom commands are created so you can see how to create your own.


Colaborate!
===========

Please, fork this project, test it, send comments and questions! Together we can make this a great test framework!

You can contact us at info@coralbits.com.
