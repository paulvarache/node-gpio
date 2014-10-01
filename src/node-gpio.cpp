#include <node.h>
#include <iostream>
#include "GPIO.h"

v8::Handle<v8::Value> Debug(const v8::Arguments& args) {
    v8::HandleScope scope;
    if (args.Length() < 1) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
        return scope.Close(v8::Undefined());
    }
    if (!args[0]->IsBoolean()) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong arguments")));
        return scope.Close(v8::Undefined());
    }
    v8::Local<v8::Boolean> param1 = args[0]->ToBoolean();
    GPIO::debug = param1->BooleanValue();
    return scope.Close(param1);
}

void init(v8::Handle<v8::Object> exports) {
    GPIO::Init(exports);
    exports->Set(v8::String::NewSymbol("setDebug"), v8::FunctionTemplate::New(Debug)->GetFunction());
}

NODE_MODULE(gpio, init);
