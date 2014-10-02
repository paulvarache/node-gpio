#include "PWM.h"
#include <node.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

v8::Persistent<v8::FunctionTemplate> PWM::pwm_constructor;

PWM::PWM(std::string num) : GPIO(num), running(false)
{

}

PWM::~PWM()
{

}

void PWM::run()
{
    while(this->running) {
        int t = (1 / this->freq) * 1000;
        int up = this->dc * t / 100;
        int down = t - up;
        std::cout << up << std::endl;
        this->write(GPIO::HIGH);
        std::cout << "UP" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(up));
        this->write(GPIO::LOW);
        std::cout << "DOWN" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(down));
    }
}

void PWM::start()
{
    this->running = true;
    this->thread = std::thread(&PWM::run, this);
}

void PWM::stop()
{
    this->running = false;
    this->thread.join();
    std::cout << "end" << std::endl;
}

void PWM::Init(v8::Handle<v8::Object> exports)
{
    v8::HandleScope scope;

    // Constructor
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New); //Binds the new
    tpl->InstanceTemplate()->SetInternalFieldCount(1); //The constructor add only one instance
    tpl->SetClassName(v8::String::NewSymbol("PWM"));

    tpl->Inherit(GPIO::constructor);

    //Accessors
    tpl->InstanceTemplate()->SetAccessor(v8::String::New("frequency"), GetFreq, SetFreq);
    tpl->InstanceTemplate()->SetAccessor(v8::String::New("dutyCycle"), GetDC, SetDC);

    //Functions
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("start"), v8::FunctionTemplate::New(Start)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("stop"), v8::FunctionTemplate::New(Stop)->GetFunction());

    //Persist constructor
    PWM::pwm_constructor = v8::Persistent<v8::FunctionTemplate>::New(tpl);

    //Register GPIO on the exports
    exports->Set(v8::String::NewSymbol("PWM"), PWM::pwm_constructor->GetFunction());
}

v8::Handle<v8::Value> PWM::New(const v8::Arguments& args)
{
    v8::HandleScope scope;
    if (args.Length() > 0 && !args[0]->IsString()) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
        return scope.Close(v8::Undefined());
    }
    PWM* pwm_instance =  args.Length() < 1 ? new PWM() : new PWM(std::string(*v8::String::Utf8Value(args[0])));

    pwm_instance->Wrap(args.This());

    return args.This();
}

v8::Handle<v8::Value> PWM::Start(const v8::Arguments& args)
{
    v8::HandleScope scope;
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(args.This());
    obj->start();
    return scope.Close(v8::Integer::New(0));
}
v8::Handle<v8::Value> PWM::Stop(const v8::Arguments& args)
{
    v8::HandleScope scope;
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(args.This());
    obj->stop();
    return scope.Close(v8::Integer::New(0));
}

v8::Handle<v8::Value> PWM::GetFreq(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    return v8::Integer::New(obj->freq);
}
void PWM::SetFreq(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    obj->freq = value->ToInteger()->IntegerValue();
}
v8::Handle<v8::Value> PWM::GetDC(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    return v8::Integer::New(obj->dc);
}
void PWM::SetDC(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
{
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    int rvalue = value->ToInteger()->IntegerValue();
    if (rvalue < 0 || rvalue > 100) {
        v8::ThrowException(v8::Exception::Error(v8::String::New("dutyCycle should be between 0 and 100")));
    } else {
        obj->dc = rvalue;
    }
}
