#include <node.h>
#include <iostream>
#include "GPIO.h"
#include "PWM.h"
#include "CapacitiveTouch.h"

void Debug(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    if (args.Length() < 1) {
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Wrong number of arguments"));
        return;
    }
    if (!args[0]->IsBoolean()) {
        isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Wrong arguments"));
        return;
    }
    v8::Local<v8::Boolean> param1 = args[0]->ToBoolean();
    GPIO::debug = param1->BooleanValue();
    args.GetReturnValue().Set(param1);
}

void Init(v8::Local<v8::Object> exports) {
    GPIO::Init(exports);
    PWM::Init(exports);
    CapacitiveTouch::Init(exports);
    NODE_SET_METHOD(exports, "setDebug", Debug);
}

NODE_MODULE(gpio, Init);
