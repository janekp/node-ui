var http = require('http');
var ui = require('ui');
var url = require('url');

// We're going to use this to authenticate the user
var session = ui.application.session;

// Create a window and show it
var window = new ui.WebWindow(400, 300, "Hello World");

window.show();

ui.application.on('ready', function() {
	window.load('http://127.0.0.1:33000/?session=' + session);
});

// Create a server
http.createServer(function(req, res) {
	var query = url.parse(req.url, true).query || { };
	
	// Authenticate the user
	if(query['session'] == session) {
		res.writeHead(200, { 'Content-Type': 'text/html', 'Content-Encoding': 'utf8' });
		res.write('<html><body><h1>Hello World!</h1></body></html>');
		res.end();
	} else {
		res.writeHead(403, { });
		res.end();
	}
}).listen(33000, '127.0.0.1');
