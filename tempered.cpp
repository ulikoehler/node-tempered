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
		Handle<Value> ret = ThrowException(String::New(err));
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
		Handle<Value> ret = ThrowException(String::New(err));
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
		return ThrowException(Exception::Error("No such device"));
	}
	//Free the device list before returning
	tempered_free_device_list(firstDev);
	//Return nothing
	return scope.Close(Undefined());
}

void init(Handle<Object> target) {
	target->Set(String::NewSymbol("init"), FunctionTemplate::New(initTempered)->GetFunction());
	target->Set(String::NewSymbol("exit"), FunctionTemplate::New(exitTempered)->GetFunction());
	target->Set(String::NewSymbol("enumerateDevices"), FunctionTemplate::New(enumerateDevices)->GetFunction());
}

NODE_MODULE(tempered, init)