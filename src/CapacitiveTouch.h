#ifndef CAPACITIVE_TOUCH
#define CAPACITIVE_TOUCH

#include "GPIO.h"
#include <node.h>

class CapacitiveTouch: public node::ObjectWrap
{
    private:
        GPIO* pinIn;
        GPIO* pinOut;
        int threshold;
        int timeout;

        static v8::Persistent<v8::FunctionTemplate> constructor;

        bool isTimeout(int);

    public:
        //Constructor destructor
        CapacitiveTouch(std::string, std::string);
        ~CapacitiveTouch();

        // Methods
        void open();
        void close();
        int getChargeCycle();
        int getSample(int);

        // V8 exposed methods
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Init(v8::Handle<v8::Object> exports);
        static void Open(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetChargeCycle(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetSample(const v8::FunctionCallbackInfo<v8::Value>& args);

        // v8 exposed properties
        static void GetTimeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
        static void SetTimeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        static void GetThreshold(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
        static void SetThreshold(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
};

#endif
