#include "PWM.h"
#include <node.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

v8::Persistent<v8::Function> PWM::pwm_constructor;

PWM::PWM(std::string num) : GPIO(num)
{

}

PWM::~PWM()
{

}

void PWM::Init(v8::Handle<v8::Object> exports)
{
    v8::HandleScope scope;

    // Constructor
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New); //Binds the new
    tpl->InstanceTemplate()->SetInternalFieldCount(1); //The constructor add only one instance
    tpl->SetClassName(v8::String::NewSymbol("PWM"));

    tpl->Inherit(GPIO::contructor);

    //Persist constructor
    PWM::pwm_constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());

    //Register GPIO on the exports
    exports->Set(v8::String::NewSymbol("PWM"), PWM::pwm_constructor);
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
