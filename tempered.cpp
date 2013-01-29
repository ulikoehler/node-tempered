#include <node.h>
#include <v8.h>
#include <cstdlib>
#include <tempered.h>

using namespace node;
using namespace v8;

static Handle<Value> initTempered(const Arguments& args) {
	char* err = NULL;
	bool status = tempered_init(&err);
	//Return the error string or NULL
	HandleScope scope;
	if(!status) {
		Handle<Value> ret = ThrowException(String::New(err));
		free(err);
		return scope.Close(ret);
	}
	return scope.Close(Undefined());
}

static Handle<Value> exitTempered(const Arguments& args) {
	char* err = NULL;
	bool status = tempered_exit(&err);
	//Return the error string or NULL
	HandleScope scope;
	if(!status) {
		Handle<Value> ret = ThrowException(String::New(err));
		free(err);
		return scope.Close(ret);
	}
	return scope.Close(Undefined());
}

static Handle<Value> enumerateDevices(const Arguments& args) {
	char* err = NULL;
	tempered_device_list* currentDev = tempered_enumerate(&err);
	tempered_device_list* firstDev = currentDev;
	//Handle any error that might have occured
	if(currentDev == NULL) {
		HandleScope scope;
		Handle<Value> ret = ThrowException(String::Concat(String::New("Can't list devices: "), String::New(err)));
		free(err);
		return scope.Close(ret);
	}
	//Build a JS object from the devices
	int i = 0;
	Local<Array> deviceList = Array::New();
	while(currentDev != NULL) {
		Local<Object> obj = Object::New();
		obj->Set(String::New("path"), String::New(currentDev->path));
		obj->Set(String::New("type"), String::New(currentDev->type_name));
		obj->Set(String::New("vendorId"), Number::New(currentDev->vendor_id));
		obj->Set(String::New("productId"), Number::New(currentDev->product_id));
		obj->Set(String::New("interfaceNumber"), Number::New(currentDev->interface_number));
		//Add the object to the array
		deviceList->Set(Number::New(i), obj);
		//Move on to the next device
		i++;
		currentDev = currentDev->next;
	}
	//Free the memory occupied by tempered
	tempered_free_device_list(firstDev);
	HandleScope scope;
	return scope.Close(deviceList);
}

/**
 * Open a TEMPered device. Call with args[0] == device path
 */
static Handle<Value> openDevice(const Arguments& args) {
	HandleScope scope;
	//We need to list the devices again to find the device list ptr
	// we want to open
	//Parse the parth argument
	char* err = NULL;
	String::AsciiValue pathArg(args[0]);
	tempered_device_list* currentDev = tempered_enumerate(&err);
	tempered_device_list* firstDev = currentDev; //Needed to free later
	if(currentDev == NULL) {
		HandleScope scope;
		Handle<Value> ret = ThrowException(String::Concat(String::New("Can't list devices: "), String::New(err)));
		free(err);
		return scope.Close(ret);
	}
	//Find the correct device
	tempered_device_list* deviceToBeOpened = NULL;
	while(currentDev != NULL) {
		//Compare the paths
		if(strcmp(currentDev->path, *pathArg) == 0) {
			deviceToBeOpened = currentDev;
			break;
		}
		//Move on to the next device
		currentDev = currentDev->next;
	}
	//Throw an exception if the specified device can't be found
	if(deviceToBeOpened == NULL) {
		return ThrowException(Exception::Error(String::New("No such device")));
	}
    //Open the device
    tempered_device* dev = tempered_open(deviceToBeOpened, &err);
    if(dev == NULL) {
		HandleScope scope;
		Handle<Value> ret = ThrowException(String::Concat(String::New("Can't open device: "), String::New(err)));
		free(err);
		return scope.Close(ret);
	}
    
	//Free the device list before continuing
	tempered_free_device_list(firstDev);
	//Return nothing
	return scope.Close(External::New(dev));
}

/**
 * Close a TEMPered device. Call with args[0] == device
 */
static Handle<Value> closeDevice(const Arguments& args) {
    tempered_device* dev = (tempered_device*)External::Unwrap(args[0]);
    tempered_close(dev);
    return Undefined();
}

/**
 * List sensors of any TEMPered device. Call with args[0] == device
 */
static Handle<Value> getSensors(const Arguments& args) {
    tempered_device* dev = (tempered_device*)External::Unwrap(args[0]);
    //Get the sensor count
    int sensorCount = tempered_get_sensor_count(dev);
    Local<String> typeTemperature = String::New("Temperature");
    Local<String> typeHumidity = String::New("Humidity");
    
	Handle<Array> sensorList = Array::New();
    for(int i = 0; i < sensorCount ; i++) {
		Local<Object> obj = Object::New();
		obj->Set(String::New("id"), Number::New(i));
        //Get the sensor type
        int sensorTypeInt = tempered_get_sensor_type(dev, i);
        if(sensorTypeInt & TEMPERED_SENSOR_TYPE_TEMPERATURE) {
    	    obj->Set(String::New("type"), typeTemperature);
        } else if(sensorTypeInt & TEMPERED_SENSOR_TYPE_HUMIDITY) {
            obj->Set(String::New("type"), typeHumidity);
        }
        sensorList->Set(Number::New(i), obj);
	}
    return sensorList;
}


/**
 * Read the sensors on the TEMPered device. Does not return the values but only updates
 * the interal value buffer of the device
 * Call with args[0] == device
 */
static Handle<Value> updateSensors(const Arguments& args) {
    tempered_device* dev = (tempered_device*)External::Unwrap(args[0]);
    bool ret = tempered_read_sensors(dev);
    if(!ret) {
    	HandleScope scope;
		Handle<Value> ret = ThrowException(String::New("Sensor read failed"));
		return scope.Close(ret);
    }
    return Undefined();
}

/**
 * Read a temperature sensors on the TEMPered device. Does not return the values but only updates
 * the interal value buffer of the device
 * Call updateSensors() before!
 * Call with args[0] == device and args[1] == sensor ID (args[1] defaults to 0)
 * @return The temperature in °C
 */
static Handle<Value> readTemperature(const Arguments& args) {
    tempered_device* dev = (tempered_device*)External::Unwrap(args[0]);
    //Parse the sensor number
    int sensorNum;
    if(args.Length() < 2) {
        sensorNum = 0;
    } else {
        sensorNum = args[1]->NumberValue();
    }
    //Read the sensor
    float tempC;
    bool ret = tempered_get_temperature(dev, sensorNum, &tempC);
    if(!ret) {
		return ThrowException(String::New("Temperature read failed"));
    }
    return Number::New(tempC);
}

/**
 * Read a humidity sensor on the TEMPered device. Does not return the values but only updates
 * the interal value buffer of the device
 * Call updateSensors() before!
 * Call with args[0] == device and args[1] == sensor ID (args[1] defaults to 0)
 * @return The relative humidity
 */
static Handle<Value> readHumidity(const Arguments& args) {
    tempered_device* dev = (tempered_device*)External::Unwrap(args[0]);
    //Parse the sensor number
    int sensorNum;
    if(args.Length() < 2) {
        sensorNum = 0;
    } else {
        sensorNum = args[1]->NumberValue();
    }
    //Read the sensor
    float relHum;
    bool ret = tempered_get_humidity(dev, sensorNum, &relHum);
    if(!ret) {
    	return ThrowException(String::New("Humidity read failed"));
    }
    return Number::New(ret);
}

void init(Handle<Object> target) {
	target->Set(String::NewSymbol("init"), FunctionTemplate::New(initTempered)->GetFunction());
	target->Set(String::NewSymbol("exit"), FunctionTemplate::New(exitTempered)->GetFunction());
	target->Set(String::NewSymbol("enumerateDevices"), FunctionTemplate::New(enumerateDevices)->GetFunction());
    target->Set(String::NewSymbol("openDevice"), FunctionTemplate::New(openDevice)->GetFunction());
    target->Set(String::NewSymbol("closeDevice"), FunctionTemplate::New(closeDevice)->GetFunction());
    target->Set(String::NewSymbol("getSensors"), FunctionTemplate::New(getSensors)->GetFunction());
    target->Set(String::NewSymbol("updateSensors"), FunctionTemplate::New(updateSensors)->GetFunction());
    target->Set(String::NewSymbol("readTemperature"), FunctionTemplate::New(readTemperature)->GetFunction());
    target->Set(String::NewSymbol("readHumidity"), FunctionTemplate::New(readHumidity)->GetFunction());
}

NODE_MODULE(tempered, init)