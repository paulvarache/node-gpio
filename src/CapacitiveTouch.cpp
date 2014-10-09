#include "CapacitiveTouch.h"
#include <iostream>

v8::Persistent<v8::FunctionTemplate> CapacitiveTouch::ct_constructor;


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
    v8::HandleScope scope;

    // Constructor definition
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(v8::String::NewSymbol("CapacitiveTouch"));

    tpl->InstanceTemplate()->SetAccessor(v8::String::New("timeout"), GetTimeout, SetTimeout);
    tpl->InstanceTemplate()->SetAccessor(v8::String::New("threshold"), GetThreshold, SetThreshold);

    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("open"), v8::FunctionTemplate::New(Open)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("close"), v8::FunctionTemplate::New(Close)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("getChargeCycle"), v8::FunctionTemplate::New(GetChargeCycle)->GetFunction());
    tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("getSample"), v8::FunctionTemplate::New(GetSample)->GetFunction());

    CapacitiveTouch::ct_constructor = v8::Persistent<v8::FunctionTemplate>::New(tpl);

    exports->Set(v8::String::NewSymbol("CapacitiveTouch"), CapacitiveTouch::ct_constructor->GetFunction());
}

v8::Handle<v8::Value> CapacitiveTouch::New(const v8::Arguments& args)
{
    v8::HandleScope scope;
    if (!args.Length() == 2)
    {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong number of arguments")));
        return scope.Close(v8::Undefined());
    }
    std::string inNum(*v8::String::Utf8Value(args[0]));
    std::string outNum(*v8::String::Utf8Value(args[1]));
    CapacitiveTouch* instance = new CapacitiveTouch(inNum, outNum);

    instance->Wrap(args.This());

    return args.This();
}

v8::Handle<v8::Value> CapacitiveTouch::Open(const v8::Arguments& args)
{
    v8::HandleScope scope;
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    obj->open();
    return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> CapacitiveTouch::Close(const v8::Arguments& args)
{
    v8::HandleScope scope;
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    obj->close();
    return scope.Close(v8::Undefined());
}

v8::Handle<v8::Value> CapacitiveTouch::GetChargeCycle(const v8::Arguments& args) {
    v8::HandleScope scope;
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    int res = obj->getChargeCycle();
    if (res < 0)
    {
        v8::ThrowException(v8::Exception::Error(v8::String::New("Cannot get charge cycle: Timeout")));
        return scope.Close(v8::Undefined());
    }
    return scope.Close(v8::Integer::New(res));
}

v8::Handle<v8::Value> CapacitiveTouch::GetSample(const v8::Arguments& args)
{
    v8::HandleScope scope;
    if(args.Length() != 1)
    {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Missing argument")));
        return scope.Close(v8::Undefined());
    }
    if(!args[0]->IsNumber())
    {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("Wrong argument")));
        return scope.Close(v8::Undefined());
    }
    int size = args[0]->ToInteger()->IntegerValue();
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(args.This());
    int res = obj->getSample(size);
    if (res < 0) {
        v8::ThrowException(v8::Exception::Error(v8::String::New("Cannot get charge cycle: timeout")));
        return scope.Close(v8::Undefined());
    }
    return scope.Close(v8::Integer::New(res));
}


/**
 * V8 EXPOSED PROPERTIES ACCESSORS
 */
v8::Handle<v8::Value> CapacitiveTouch::GetTimeout(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    return v8::Integer::New(obj->timeout);
}
void CapacitiveTouch::SetTimeout(v8::Local<v8::String> property, v8::Local<v8::Value> value,  const v8::AccessorInfo& info)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    obj->timeout = value->ToInteger()->IntegerValue();
}
v8::Handle<v8::Value> CapacitiveTouch::GetThreshold(v8::Local<v8::String> property, const v8::AccessorInfo& info)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    return v8::Integer::New(obj->threshold);
}
void CapacitiveTouch::SetThreshold(v8::Local<v8::String> property, v8::Local<v8::Value> value,  const v8::AccessorInfo& info)
{
    CapacitiveTouch* obj = node::ObjectWrap::Unwrap<CapacitiveTouch>(info.Holder());
    obj->threshold = value->ToInteger()->IntegerValue();
}
