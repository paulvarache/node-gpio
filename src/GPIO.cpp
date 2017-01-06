#include <node.h>
#include "GPIO.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

v8::Persistent<v8::FunctionTemplate> GPIO::constructor;
std::string GPIO::IN = "in";
std::string GPIO::OUT = "out";
int GPIO::HIGH = 1;
int GPIO::LOW = 0;
bool GPIO::debug = false;

GPIO::GPIO(std::string num) : pin_num(num), opened(false)
{

}

GPIO::~GPIO()
{

}


void GPIO::Init(v8::Local<v8::Object> exports)
{
    v8::Isolate* isolate = exports->GetIsolate();
    // Constructor
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New); //Binds the new
    tpl->InstanceTemplate()->SetInternalFieldCount(1); //The constructor add only one instance
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "GPIO"));

    //Accessors
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "num"), GetNum);

    //Methods
    NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setMode", SetMode);
    NODE_SET_PROTOTYPE_METHOD(tpl, "write", Write);
    NODE_SET_PROTOTYPE_METHOD(tpl, "read", Read);
    NODE_SET_PROTOTYPE_METHOD(tpl, "toggle", Toggle);

    GPIO::constructor.Reset(isolate, tpl);

    //Register GPIO on the exports
    exports->Set(v8::String::NewFromUtf8(isolate, "GPIO"), tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "IN"), v8::String::NewFromUtf8(isolate, GPIO::IN.c_str()));
    exports->Set(v8::String::NewFromUtf8(isolate, "OUT"), v8::String::NewFromUtf8(isolate, GPIO::OUT.c_str()));
    exports->Set(v8::String::NewFromUtf8(isolate, "HIGH"), v8::Integer::New(isolate, GPIO::HIGH));
    exports->Set(v8::String::NewFromUtf8(isolate, "LOW"), v8::Integer::New(isolate, GPIO::LOW));
}

void GPIO::New(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    // Invoked as constructor: `new MyObject(...)`
    if (args.Length() > 0 && !args[0]->IsString()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }
    GPIO* gpio_instance =  args.Length() < 1 ? new GPIO() : new GPIO(std::string(*v8::String::Utf8Value(args[0])));

    gpio_instance->Wrap(args.This());

    args.GetReturnValue().Set(args.This());
}

void GPIO::GetNum(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Isolate* isolate = info.GetIsolate();
    GPIO* obj = node::ObjectWrap::Unwrap<GPIO>(info.Holder());
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, obj->pin_num.c_str()));
}

void GPIO::log(std::string msg)
{
    if (GPIO::debug) {
        std::cout << msg << std::endl;
    }
}

int GPIO::WriteValue(std::string path, std::string val)
{
    std::ofstream file(path.c_str());
    if (!file) {
        return -1;
    }
    file << val;
    file.close();
    return 0;
}

int GPIO::open()
{
    this->opened = true;
    return this->WriteValue("/sys/class/gpio/export", this->pin_num);
}

int GPIO::close()
{
    this->opened = false;
    return this->WriteValue("/sys/class/gpio/unexport", this->pin_num);
}

int GPIO::write(int value) {
    if (!this->opened) {
        return -1;
    }
    std::string path = "/sys/class/gpio/gpio" + this->pin_num + "/value";
    std::ostringstream ss;
    ss << value;
    return this->WriteValue(path, ss.str());
}

int GPIO::read()
{
    if (!this->opened) {
        return -1;
    }
    std::string path = "/sys/class/gpio/gpio" + this->pin_num + "/value";
    return this->ReadValue(path);
}

int GPIO::setMode(std::string mode) {
    std::string path = "/sys/class/gpio/gpio" + this->pin_num + "/direction";
    this->mode = mode;
    return this->WriteValue(path, mode);
}

int GPIO::ReadValue(std::string path)
{
    std::ifstream file(path.c_str());
    std::string line;
    if (!file) {
        return -1;
    }
    std::getline(file, line);
    file.close();
    return atoi(line.c_str());
}

void GPIO::Open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    int res = obj->open();
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to open GPIO " + obj->pin_num + ".";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    obj->log("GPIO " + obj->pin_num + " opened.");
    args.GetReturnValue().Set(v8::Integer::New(isolate, res));
}

void GPIO::Close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    int res = obj->close();
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to close GPIO " + obj->pin_num + ".";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    obj->log("GPIO " + obj->pin_num + " closed.");
    args.GetReturnValue().Set(v8::Integer::New(isolate, res));
}

void GPIO::SetMode(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    if (!obj->opened) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not opened.";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    if (args.Length() < 1) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    if (!args[0]->IsString()) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }
    std::string mode(*v8::String::Utf8Value(args[0]));
    int res = obj->setMode(mode);
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to change GPIO " + obj->pin_num + " mode.";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    obj->log("GPIO " + obj->pin_num + " mode changed to " + mode + ".");
    args.GetReturnValue().Set(v8::Integer::New(isolate, res));
}


void GPIO::Write(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    if (args.Length() < 1) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    if (!args[0]->IsNumber()) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }
    v8::Local<v8::Integer> param1 = args[0]->ToInteger();
    int value = param1->IntegerValue();
    int res = obj->write(value);
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to change GPIO " + obj->pin_num + " value.";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    std::ostringstream ss;
    ss << value;
    obj->log("GPIO " + obj->pin_num + " value changed to " + ss.str() + ".");
    args.GetReturnValue().Set(v8::Integer::New(isolate, res));
}

void GPIO::Read(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    if (!obj->opened) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not opened.";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    if (obj->mode == GPIO::OUT) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not in the right mode";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    int res = obj->read();
    std::ostringstream ss;
    ss << res;
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to change GPIO " + obj->pin_num + " value.";
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, err_msg.c_str())));
        return;
    }
    obj->log("GPIO " + obj->pin_num + " value changed to " + ss.str() + ".");
    args.GetReturnValue().Set(v8::Integer::New(isolate, res));
}

void GPIO::Toggle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    std::string path = "/sys/class/gpio/gpio" + obj->pin_num + "/value";
    int value = obj->ReadValue(path);
    std::ostringstream ss;
    ss << !value;
    obj->WriteValue(path, ss.str());
    args.GetReturnValue().Set(v8::Integer::New(isolate, 0));
}
