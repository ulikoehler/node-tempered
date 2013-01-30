var timers = require("timers")
var TEMPeredDevice = require("../tempered.js").TEMPeredDevice

var device = new TEMPeredDevice();
timers.setInterval(function() {
    console.dir(device.readTemperature());
}, 1000);