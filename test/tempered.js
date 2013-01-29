var util = require('util')
var tempered = require('../build/Debug/tempered');

console.dir("Initing...")
console.log(tempered.init());
console.dir("Listing...")
var devices = tempered.enumerateDevices()
console.log(devices);
console.dir("Opening device...")
var device = tempered.openDevice(devices[0].path)
console.dir("Listing sensors...")
var sensors = tempered.getSensors(device);
console.log(sensors);
console.dir("Updating sensor data...")
tempered.updateSensors(device)
console.dir("Reading temperature...")
console.dir(tempered.readTemperature(device) + " °C")
console.dir("Closing device...")
var device = tempered.closeDevice(device)
console.dir("Exiting...")
console.log(tempered.exit());
