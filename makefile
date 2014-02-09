install-logs: build
	pebble install --logs

browserify:
	browserify src/js/_pebble-js-app.js -o src/js/pebble-js-app.js

build: browserify
	pebble build

install: build
	pebble install
	
	
