#include <node.h>
#include <v8.h>
#include <cstdlib>
#include <tempered.h>

using namespace node;
using namespace v8;

static Handle<Value> initTempered(const Arguments& args) {
	char* err;
	bool status = tempered_init(&err);
	//Return the error string or NULL
	HandleScope scope;
	if(!status) {
		Handle<Value> ret = String::New(err);
		free(err);
		return scope.Close(ret);
	}
	return scope.Close(Undefined());
}

static Handle<Value> exitTempered(const Arguments& args) {
	char* err;
	bool status = tempered_exit(&err);
	//Return the error string or NULL
	HandleScope scope;
	if(!status) {
		Handle<Value> ret = String::New(err);
		free(err);
		return scope.Close(ret);
	}
	return scope.Close(Undefined());
}

static Handle<Value> enumerateDevices(const Arguments& args) {
	char* err;
	tempered_device_list* currentDev = tempered_enumerate(&err);
	tempered_device_list* firstDev = currentDev;
	//Handle any error that might have occured
	/*if(err != NULL && strlen(err) > 0) {
		HandleScope scope;
		Handle<Value> ret = String::New(err);
		//free(err);
		return scope.Close(ret);
	}*/
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

void init(Handle<Object> target) {
	target->Set(String::NewSymbol("init"), FunctionTemplate::New(initTempered)->GetFunction());
	target->Set(String::NewSymbol("exit"), FunctionTemplate::New(exitTempered)->GetFunction());
	target->Set(String::NewSymbol("enumerateDevices"), FunctionTemplate::New(enumerateDevices)->GetFunction());
}

NODE_MODULE(tempered, init)