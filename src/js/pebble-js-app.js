Pebble.addEventListener("ready", function(e) {
		console.log("JS Ready");
		pollStatus();
});

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


function pollStatus() {
	console.log("polling status");
	ajax('http://admin:admin@192.168.15.17/rest/status/20%20B9%20BA%201', function(data) {
		var status = data.match(/formatted="(.*?)"/)[1];	
		currentStatus = status;
		console.log("status is "+status);
		Pebble.sendAppMessage({'status':status});
	});
}


function turnOn() {
	if(currentStatus == "Off") {
		console.log("via js - turning on");
		ajax('http://admin:admin@192.168.15.17/rest/nodes/20%20B9%20BA%201/cmd/DON', function(data) {
			currentStatus = "On";
			console.log("return from API - on");
			Pebble.sendAppMessage({'status':"On"});

		});
	}
}

function turnOff() {
	if(currentStatus == "On") {
		console.log("via js - turning off");
		ajax('http://admin:admin@192.168.15.17/rest/nodes/20%20B9%20BA%201/cmd/DOF', function(data) {
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
