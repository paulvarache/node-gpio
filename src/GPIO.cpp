#include <node.h>
#include "GPIO.h"
#include <fstream>
#include <iostream>
#include <sstream>

v8::Persistent<v8::Function> GPIO::constructor;
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

void GPIO::Init(v8::Handle<v8::Object> exports)
{
    v8::HandleScope scope;

    // Constructor
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New); //Binds the new
    tpl->InstanceTemplate()->SetInternalFieldCount(1); //The constructor add only one instance
    tpl->SetClassName(v8::String::NewSymbol("GPIO"));

    //Accessors
    tpl->InstanceTemplate()->SetAccessor(v8::String::New("num"), GetNum);

    //Methods
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("open"), v8::FunctionTemplate::New(Open)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("close"), v8::FunctionTemplate::New(Close)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("setMode"), v8::FunctionTemplate::New(SetMode)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("write"), v8::FunctionTemplate::New(Write)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("read"), v8::FunctionTemplate::New(Read)->GetFunction());

    //Persist constructor
    GPIO::constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());

    //Register GPIO on the exports
    exports->Set(v8::String::NewSymbol("GPIO"), GPIO::constructor);
    exports->Set(v8::String::NewSymbol("IN"), v8::String::New(GPIO::IN.c_str()));
    exports->Set(v8::String::NewSymbol("OUT"), v8::String::New(GPIO::OUT.c_str()));
    exports->Set(v8::String::NewSymbol("HIGH"), v8::Integer::New(GPIO::HIGH));
    exports->Set(v8::String::NewSymbol("LOW"), v8::Integer::New(GPIO::LOW));
}

v8::Handle<v8::Value> GPIO::New(const v8::Arguments& args)
{
    v8::HandleScope scope;
    if (args.Length() > 0 && !args[0]->IsString()) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
        return scope.Close(v8::Undefined());
    }
    GPIO* gpio_instance =  args.Length() < 1 ? new GPIO() : new GPIO(std::string(*v8::String::Utf8Value(args[0])));

    gpio_instance->Wrap(args.This());

    return args.This();
}

v8::Handle<v8::Value> GPIO::GetNum(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    GPIO* obj = node::ObjectWrap::Unwrap<GPIO>(info.Holder());
    return v8::String::New(obj->pin_num.c_str());
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

int GPIO::ReadValue(std::string path)
{
    std::ifstream file(path.c_str());
    std::string line;
    if (!file) {
        return -1;
    }
    getline(file, line);
    file.close();
    return atoi(line.c_str());
}

v8::Handle<v8::Value> GPIO::Open(const v8::Arguments& args)
{
    v8::HandleScope scope;
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    int res = obj->WriteValue("/sys/class/gpio/export", obj->pin_num);
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to open GPIO " + obj->pin_num + ".";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    obj->opened = true;
    obj->log("GPIO " + obj->pin_num + " opened.");
    return scope.Close(v8::Integer::New(res));
}

v8::Handle<v8::Value> GPIO::Close(const v8::Arguments& args)
{
    v8::HandleScope scope;
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    int res = obj->WriteValue("/sys/class/gpio/unexport", obj->pin_num);
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to close GPIO " + obj->pin_num + ".";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    obj->opened = false;
    obj->log("GPIO " + obj->pin_num + " closed.");
    return scope.Close(v8::Integer::New(res));
}

v8::Handle<v8::Value> GPIO::SetMode(const v8::Arguments& args) {
    v8::HandleScope scope;
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    if (!obj->opened) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not opened.";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    if (args.Length() < 1) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
        return scope.Close(v8::Undefined());
    }
    if (!args[0]->IsString()) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
        return scope.Close(v8::Undefined());
    }
    std::string mode(*v8::String::Utf8Value(args[0]));
    std::string path = "/sys/class/gpio/gpio" + obj->pin_num + "/direction";
    int res = obj->WriteValue(path, mode);
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to change GPIO " + obj->pin_num + " mode.";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    obj->mode = mode;
    obj->log("GPIO " + obj->pin_num + " mode changed to " + mode + ".");
    return scope.Close(v8::Integer::New(res));
}


v8::Handle<v8::Value> GPIO::Write(const v8::Arguments& args) {
    v8::HandleScope scope;
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    if (!obj->opened) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not opened.";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    if (obj->mode == GPIO::IN) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not in the right mode";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    if (args.Length() < 1) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
        return scope.Close(v8::Undefined());
    }
    if (!args[0]->IsNumber()) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
        return scope.Close(v8::Undefined());
    }
    v8::Local<v8::Integer> param1 = args[0]->ToInteger();
    int value = param1->IntegerValue();
    std::string path = "/sys/class/gpio/gpio" + obj->pin_num + "/value";
    std::ostringstream ss;
    ss << value;
    int res = obj->WriteValue(path, ss.str());
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to change GPIO " + obj->pin_num + " value.";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    obj->log("GPIO " + obj->pin_num + " value changed to " + ss.str() + ".");
    return scope.Close(v8::Integer::New(res));
}

v8::Handle<v8::Value> GPIO::Read(const v8::Arguments& args) {
    v8::HandleScope scope;
    GPIO* obj = ObjectWrap::Unwrap<GPIO>(args.This());
    if (!obj->opened) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not opened.";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    if (obj->mode == GPIO::OUT) {
        std::string err_msg = "GPIO " + obj->pin_num + " is not in the right mode";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    std::string path = "/sys/class/gpio/gpio" + obj->pin_num + "/value";
    int res = obj->ReadValue(path);
    std::ostringstream ss;
    ss << res;
    if (res < 0) {
        std::string err_msg = "OPERATION FAILED: Unable to change GPIO " + obj->pin_num + " value.";
        v8::ThrowException(v8::Exception::Error(v8::String::New(err_msg.c_str())));
        return scope.Close(v8::Undefined());
    }
    obj->log("GPIO " + obj->pin_num + " value changed to " + ss.str() + ".");
    return scope.Close(v8::Integer::New(res));
}
