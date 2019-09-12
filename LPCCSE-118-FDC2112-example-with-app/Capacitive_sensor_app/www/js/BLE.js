var connected_device = null;
var scantimeout;

function map( x,  in_min,  in_max,  out_min,  out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

function startScan()	{	
	ble.isEnabled(
		function(){
				// Bluetooth is enabled
			},
			function(){
				// Bluetooth not yet enabled so we try to enable it
				ble.enable(
					function(){
					// bluetooth now enabled
				},
				function(err){
					alert('Cannot enable bluetooth');
				}
				);
			}
			);

	var parentElement = document.getElementById('deviceready');
	var deviceElement = parentElement.querySelector('.device');
	deviceElement.textContent = "No DLG-CAP found";
	ble.startScan([], function(device) {
		console.log(JSON.stringify(device));
		if(device.name == "DLG-CAP"){
			deviceElement.textContent = "DLG-CAP found";
			ble.stopScan();
			clearTimeout(scantimeout);
			ble.connect(device.id, connectCallback, disconnectCallback);				
		}
	}, 
	function(err){console.log("Error starting scan")});

	scantimeout = setTimeout(stopScan_callback, 5000);
}

function connectCallback(device){
	console.log("Connected");
	console.log(JSON.stringify(device));
	connected_device = device;
	var parentElement = document.getElementById('deviceready');
	var deviceElement = parentElement.querySelector('.device');
	deviceElement.textContent = "Connected to DLG-CAP";
	var element = document.querySelector('.pre_connected_class');
	element.setAttribute('style', 'display:none;'); //Hide the scan button
	ble.startNotification(device.id, "502678e8-dc28-749a-ff4d-a546a6280f3b", "5b3d31fe-ba78-9c90-d24b-a78de25e41e0", notification_callback, null);
	element = document.querySelector('.connected_class');
	element.setAttribute('style', 'display:block;'); //Show the disconnect button
}

function disconnectCallback(device){
	console.log("Disconnected");
	connected_device = null;
	var parentElement = document.getElementById('deviceready');
	var deviceElement = parentElement.querySelector('.device');
	deviceElement.textContent = "Disconnected";
	var element = document.querySelector('.pre_connected_class');
	element.setAttribute('style', 'display:block;'); //Show the scan button
	element = document.querySelector('.connected_class');
	element.setAttribute('style', 'display:none;'); //Hide the disconnect button
}


function stopScan_callback() { 
	console.log("Scan complete");
	ble.stopScan();
	var parentElement = document.getElementById('deviceready');
	var deviceElement = parentElement.querySelector('.device');
	deviceElement.textContent = "Stopped scanning"; 
}

function notification_callback(buffer) {
    // Decode the ArrayBuffer into a typed Array based on the data you expect
    var data = String.fromCharCode.apply(null, new Uint8Array(buffer));
    //console.log(data);
    //document.getElementById("capacitance_data").innerHTML = "Capacitance value: " + data;
    var touchPressure = parseInt(data);
    var touchPressureNormalized = map(touchPressure, 2450, 0, 0, 1);
	var element = document.getElementById('circle');
	element.style.width  = "" + 50 * touchPressureNormalized + "px";//Set circle width
	element.style.height = "" + 50 * touchPressureNormalized + "px";//Set circle height
	element.style.borderWidth = "" + (50-50 * touchPressureNormalized)/2 + "px";//Set circle border
}
