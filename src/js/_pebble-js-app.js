var xml2js = require('xml2js');
var servername = "";
var username = "";
var password = "";
var node = "";

Pebble.addEventListener("ready", function(e) {
		console.log("JS Ready");
		init();
});

function buildUrl(){
	var url = 'http://'+username+':'+password+'@'+servername;
	url += '/rest/nodes/'+node.replace(/(.{2})(.{2})(.{2})(.+)/, '$1%20$2%20$3%20$4').toUpperCase();
	return url;
}

function init(){
	servername = window.localStorage.getItem("server");
	username = window.localStorage.getItem("username");
	password = window.localStorage.getItem("password");
	node = window.localStorage.getItem("node").replace(/\s/g,'').toUpperCase();
	if(servername == "" || username == "" || password == "" || node == ""){
		Pebble.sendAppMessage({'device':'Need Configuration'});
	} else {
		pollNode();
	}
}

Pebble.addEventListener("appmessage",
  function(e) {
	  console.log("message received in js");
 	  if(e.payload.command == "1"){
		  if(currentStatus == "On"){
			  turnOff();
		  } else {
			  turnOn();
		  }
	  } 
  }
);

Pebble.addEventListener("showConfiguration", function() {
	console.log("showing configuration");
	Pebble.openURL('http://www.chasepeeler.com/pebble-config.php?server='+servername+'&username='+username+'&password='+password+'&node='+node);
});

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("configuration closed");
	// webview closed
	var options = JSON.parse(decodeURIComponent(e.response));
	window.localStorage.setItem('servername', options.server);
	window.localStorage.setItem('username', options.username);
	window.localStorage.setItem('password',options.password);
	window.localStorage.setItem('node', options.node.replace(/\s/g,''));
	init();
});



function getItems(){
	ajax('http://admin:admin@192.168.15.17/rest/nodes',function(data){
		console.log(data);
//		var parser = new xml2js.Parser();
//		parser.parseString(data,function(err,result){
//			var js = JSON.stringify(result);
//			console.log(js);
//		});
	});
}


var currentStatus;

function ajax(url,callback) {
	var req = new XMLHttpRequest();
	req.open('GET', url, true);
	req.onload = function(e) {
		if(req.readyState == 4 && req.status == 200) {
			if(req.status == 200) {
				callback(req.responseText);
			} else {
				console.log("Error");
			}
		}
	};
	console.log("Ajax Request: "+url);
	req.send(null);
}


function pollNode() {
	console.log("polling node");
	url = buildUrl();
	ajax(url, function(data) {
		var parser = new xml2js.Parser();
		parser.parseString(data,function(err,result){
			var d = JSON.parse(JSON.stringify(result));
			var name = d.nodeInfo.node[0].name;
			var status = d.nodeInfo.node[0].property[0].$.formatted;
			currentStatus = status;
			Pebble.sendAppMessage({'status':status,'device':name});
		});
	});
}


function turnOn() {
	if(currentStatus == "Off") {
		console.log("via js - turning on");
		var url = buildUrl() + '/cmd/DON';
		ajax(url, function(data) {
			currentStatus = "On";
			console.log("return from API - on");
			Pebble.sendAppMessage({'status':"On"});

		});
	}
}

function turnOff() {
	if(currentStatus == "On") {
		console.log("via js - turning off");
		var url = buildUrl() + '/cmd/DOF';
		ajax(url, function(data) {
			currentStatus = "Off";
			console.log("return from API -off");
			Pebble.sendAppMessage({'status':"Off"});
		});
	}
}
/*
simply.title("ISY Loading");
displayStatus();
*/
