#include "CapacitiveTouch.h"
#include <iostream>

v8::Persistent<v8::FunctionTemplate> CapacitiveTouch::constructor;


/**
 * CLASS METHODS
 */
CapacitiveTouch::CapacitiveTouch(std::string pinInNum, std::string pinOutNum): threshold(30), timeout(10000)
{
    this->pinIn = new GPIO(pinInNum);
    this->pinOut = new GPIO(pinOutNum);
}
CapacitiveTouch::~CapacitiveTouch()
{
    delete this->pinIn;
    delete this->pinOut;
}
void CapacitiveTouch::open()
{
    this->pinIn->open();
    this->pinOut->open();
}
void CapacitiveTouch::close()
{
    this->pinIn->close();
    this->pinOut->close();
}
int CapacitiveTouch::getChargeCycle()
{
    this->pinIn->setMode(GPIO::IN);
    this->pinOut->setMode(GPIO::OUT);

    int t = 0;

    this->pinOut->write(GPIO::HIGH);

    // Lets out a high state on the out pin and wait for the in pin to get it
    while(this->pinIn->read() != GPIO::HIGH)
    {
        t++;
        if (this->isTimeout(t)) {return -1;}
    }

    // Lets fully charge the capacitor
    this->pinIn->setMode(GPIO::OUT);
    this->pinIn->write(GPIO::HIGH);
    this->pinIn->setMode(GPIO::IN);

    //And now count the discharge
    this->pinOut->write(GPIO::LOW);
    while(this->pinIn->read() != GPIO::LOW)
    {
        t++;
        if (this->isTimeout(t)) {return -1;}
    }
    return t;
}
int CapacitiveTouch::getSample (int size)
{
    int total = 0;
    int step = 0;
    for(int i = 0; i < size; i++)
    {
        step = this->getChargeCycle();
        if (step < 0) {return -1;}
        total += step;
    }
    return total;
}
bool CapacitiveTouch::isTimeout (int value)
{
    return value >= this->timeout;
}

/**
 * V8 EXPOSED METHODS
 */
void CapacitiveTouch::Init(v8::Handle<v8::Object> exports)
{
    v8::Isolate* isolate = exports->GetIsolate();

    // Constructor definition
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "CapacitiveTouch"));

    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "timeout"), GetTimeout, SetTimeout);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "threshold"), GetThreshold, SetThreshold);

    NODE_SET_PROTOTYPE_METHOD(tpl, "open", Open);
    NODE_SET_PROTOTYPE_METHOD(tpl, "close", Close);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getChargeCycle", GetChargeCycle);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getSample", GetSample);

    CapacitiveTouch::constructor.Reset(isolate, tpl);

    exports->Set(v8::String::NewFromUtf8(isolate, "CapacitiveTouch"), tpl->GetFunction());
}

void CapacitiveTouch::New(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (!args.Length() == 2)
    {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    std::string inNum(*v8::String::Utf8Value(args[0]));
    std::string outNum(*v8::String::Utf8Value(args[1]));
    CapacitiveTouch* instance = new CapacitiveTouch(inNum, outNum);

    instance->Wrap(args.This());

    args.GetReturnValue().Set(args.This());
}

void CapacitiveTouch::Open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    obj->open();
}

void CapacitiveTouch::Close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    obj->close();
}

void CapacitiveTouch::GetChargeCycle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    int res = obj->getChargeCycle();
    if (res < 0)
    {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Cannot get charge cycle: Timeout")));
        return;
    }
    args.GetReturnValue().Set(v8::Integer::New(isolate, res));
}

void CapacitiveTouch::GetSample(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (args.Length() != 1)
    {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Missing argument")));
        return;
    }
    if (!args[0]->IsNumber())
    {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong argument")));
        return;
    }
    int size = args[0]->ToInteger()->IntegerValue();
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    int res = obj->getSample(size);
    if (res < 0) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "Cannot get charge cycle: timeout")));
        return;
    }
    args.GetReturnValue().Set(v8::Integer::New(isolate, res));
}


/**
 * V8 EXPOSED PROPERTIES ACCESSORS
 */
void CapacitiveTouch::GetTimeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Isolate* isolate = info.GetIsolate();
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->timeout));
}
void CapacitiveTouch::SetTimeout(v8::Local<v8::String> property, v8::Local<v8::Value> value,  const v8::PropertyCallbackInfo<void> &info)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    obj->timeout = value->ToInteger()->IntegerValue();
}
void CapacitiveTouch::GetThreshold(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Isolate* isolate = info.GetIsolate();
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->threshold));
}
void CapacitiveTouch::SetThreshold(v8::Local<v8::String> property, v8::Local<v8::Value> value,  const v8::PropertyCallbackInfo<void> &info)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    obj->threshold = value->ToInteger()->IntegerValue();
}
