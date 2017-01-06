#include "PWM.h"
#include <node.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

v8::Persistent<v8::FunctionTemplate> PWM::constructor;

PWM::PWM(std::string num) : GPIO(num), running(false)
{
    GPIO::setMode(GPIO::OUT);
}

PWM::~PWM()
{

}

int PWM::setMode(std::string mode)
{
    return -1;
}

void PWM::run()
{
    while(this->running) {
        int t = 1000 / this->freq;
        int up = this->dc * t / 100;
        int down = t - up;
        this->write(GPIO::HIGH);
        std::this_thread::sleep_for(std::chrono::milliseconds(up));
        this->write(GPIO::LOW);
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
}

void PWM::Init(v8::Local<v8::Object> exports)
{
    v8::Isolate* isolate = exports->GetIsolate();

    // Constructor
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New); //Binds the new
    tpl->InstanceTemplate()->SetInternalFieldCount(1); //The constructor add only one instance
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "PWM"));

    v8::Local<v8::FunctionTemplate> parent = v8::Local<v8::FunctionTemplate>::New(isolate, GPIO::constructor);

    tpl->Inherit(parent);

    //Accessors
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "frequency"), GetFreq, SetFreq);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "dutyCycle"), GetDC, SetDC);

    //Functions
    NODE_SET_PROTOTYPE_METHOD(tpl, "start", Start);
    NODE_SET_PROTOTYPE_METHOD(tpl, "stop", Stop);

    //Persist constructor
    PWM::constructor.Reset(isolate, tpl);

    //Register PWM on the exports
    exports->Set(v8::String::NewFromUtf8(isolate, "PWM"), tpl->GetFunction());
}

void PWM::New(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (args.Length() > 0 && !args[0]->IsString()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }
    PWM* pwm_instance =  args.Length() < 1 ? new PWM() : new PWM(std::string(*v8::String::Utf8Value(args[0])));

    pwm_instance->Wrap(args.This());

    args.GetReturnValue().Set(args.This());
}

void PWM::Start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(args.This());
    obj->start();
    args.GetReturnValue().Set(v8::Integer::New(isolate, 0));
}
void PWM::Stop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(args.This());
    obj->stop();
    args.GetReturnValue().Set(v8::Integer::New(isolate, 0));
}

void PWM::GetFreq(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Isolate* isolate = info.GetIsolate();
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->freq));
}
void PWM::SetFreq(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
{
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    obj->freq = value->ToInteger()->IntegerValue();
}
void PWM::GetDC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Isolate* isolate = info.GetIsolate();
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->dc));
}
void PWM::SetDC(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
{
    v8::Isolate* isolate = info.GetIsolate();
    PWM* obj = node::ObjectWrap::Unwrap<PWM>(info.Holder());
    int rvalue = value->ToInteger()->IntegerValue();
    if (rvalue < 0 || rvalue > 100) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "dutyCycle should be between 0 and 100")));
    } else {
        obj->dc = rvalue;
    }
}
