var util = require('util')
var tempered = require('../build/Debug/tempered');

console.log(util.inspect(tempered, true, null));
console.dir("");
console.dir("Initing...")
console.log(tempered.init());
console.dir("Listing...")
var devices = tempered.enumerateDevices()
console.log();
console.dir("Exiting...")
console.log(tempered.exit());
