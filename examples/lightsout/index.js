var ui = require('ui');

var game = [
	[ 0, 0, 0 ],
	[ 0, 0, 0 ],
	[ 0, 0, 0 ]
];

function isGameOver() {
	for(var i = 0; i < 3; i++) {
		for(var j = 0; j < 3; j++) {
			if(!game[i][j]) {
				return false;
			}
		}
	}
	
	return true;
}

function _toggle(row, col) {
	if(row >= 0 && row <= 2 && col >= 0 && col <= 2) {
		var state = (game[row][col]) ? 0 : 1;
		
		game[row][col] = state;
		
		return 'change(' + row + ',' + col + ',' + state + ');';
	}
	
	return "";
}

function toggle(row, col, sw, cb) {
	var script = _toggle(row, col);
	
	if(sw) {
		script += _toggle(row - 1, col);
		script += _toggle(row + 1, col);
		script += _toggle(row, col - 1);
		script += _toggle(row, col + 1);
	}
	
	window.view.exec(script, cb);
}

var window = new ui.WebWindow(400, 400, "Lights Out!");

window.load("index.html");

window.view.on('ready', function() {
	toggle(1, 2, false, function() {
		window.show();
	});
});

window.view.on('data', function(msg) {
	if(!isGameOver()) {
		var pos = msg.split('/');
		
		toggle(parseInt(pos[0]), parseInt(pos[1]), true, function() {
			if(isGameOver()) {
				window.view.exec('gameover();');
			}
		});
	}
});
