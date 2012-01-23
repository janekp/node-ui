var ui = require('ui');

// Create a window and show it
var window = new ui.WebWindow(400, 300, "Hello World");

window.load(function(stream) { stream.write("<html><body><h1>Hello World! :)</h1></body></html>"); });
window.show();

// Attach an event listener
window.on('close', function() {
	console.log('Window was closed');
});

// Attach global event listeners
ui.application.on('activate', function() {
	window.title = "Hello World [ACTIVE]";
});

ui.application.on('deactivate', function() {
	window.title = "Hello World";
});

ui.application.on('exit', function() {
	console.log('Application exited');
});
