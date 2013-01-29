tempered = require "build/Release/tempered"

temperedInited = false

class TEMPeredDevice
    constructor: (deviceNum = 0) ->
        if not temperedInited
            tempered.init()
            temperedInited = true
        @device = tempered.openDevice(deviceNum)
        #Read the sensors to be able to check sensor type later without needing
        # to query the device
        @sensors = tempered.getSensors(@device)
    close: () =>
        tempered.exit()
        temperedInited = false
    readTemperature: (sensorNum = 0) =>
        ###
        Read the temperature from the configured device
        @param sensorNum The sensor ID, 0 for devices with only one sensor
        ###
        tempered.updateSensors(@device)
        return tempered.readTemperature(@device, sensorNum)
    readTemperature: (sensorNum = 0) =>
        ###
        Read the humidity from the configured device
        @param sensorNum The sensor ID, 0 for devices with only one sensor
        ###
        tempered.updateSensors(@device)
        return tempered.readTemperature(@device, sensorNum)


module.exports = {TEMPeredDevice: TEMPeredDevice}