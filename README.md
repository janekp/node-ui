Create native Desktop applications with [NodeJS](http://nodejs.org)
===

node-ui adds additional GUI core modules to node. The goal is to provide a good cross-platform wrapper
with a clear migration path towards pure web-apps (eg. for apps that require a lot of bandwidth).

node is embed as a static or dynamic library so no child processes are needed.
Eventually this will allow to create applications written in Javascript
to be distributed as a single self-contained MyApplication.exe or MyApplication.app

### Dependencies:

[NodeJS + dynamic modules](http://github.com/janekp/node)

### OS Support:

* Mac OS X
* Windows - [MISSING]
* Linux - [MISSING]

### Features:

* Compatible with node cmdline tool
* Windows, views, web-views
* Menus [SOON]
* package.json to customize application name, icon and URL schemes [SOON]
* Alert, save and open dialogs [SOON]
* Built-in documents and undo management [SOON]
* Application generation for distribution [MISSING]

### Hello world:

main.js:
	
	var ui = require('ui');
	
	var window = new ui.WebWindow(400, 300, "Hello World [ACTIVE]");
	
	window.load(function(stream) { stream.write("<html><body><h1>Hello World! :)</h1></body></html>"); });
	window.show();

Launch it:

	node-ui main.js

![helloworld.png](https://github.com/janekp/node-ui/raw/master/doc/images/helloworld.png "Hello World")