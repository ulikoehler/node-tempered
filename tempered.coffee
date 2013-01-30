tempered = require "./build/Release/tempered"

temperedInited = false
temperedRefCount = 0 #If this is decremented to 0, the tempered library resources are freed

class TEMPeredDevice
    constructor: (deviceNum = 0) ->
        if not temperedInited
            tempered.init()
            temperedInited = true
        temperedRefCount += 1
        @device = tempered.openDevice(deviceNum)
        #Read the sensors to be able to check sensor type later without needing
        # to query the device
        @sensors = tempered.getSensors(@device)
    close: () =>
        ###
        Free the resources occupied by this device
        ###
        tempered.closeDevice(@device)
        temperedRefCount -= 1
        if temperedRefCount == 0
            tempered.exit()
            temperedInited = false
    sensorCount: () =>
        ###
        @return The sensor count for the current device
        ###
        return @sensors.length
    getSensorType: (sensorNum) =>
        ###
        @return Null if there is no such sensor, "Temperature" or "Humidity" else
        ###
        if sensorNum > (@sensors.length - 1)
            return null
        return @sensors[sensorNum].type
    readTemperature: (sensorNum = 0) =>
        ###
        Read the temperature from the configured device
        @param sensorNum The sensor ID, 0=default for devices with only one sensor
        ###
        tempered.updateSensors(@device)
        return tempered.readTemperature(@device, sensorNum)
    readHumidity: (sensorNum = 0) =>
        ###
        Read the humidity from the configured device
        @param sensorNum The sensor ID, 0=default for devices with only one sensor
        ###
        tempered.updateSensors(@device)
        return tempered.readHumidity(@device, sensorNum)


module.exports = {TEMPeredDevice: TEMPeredDevice}