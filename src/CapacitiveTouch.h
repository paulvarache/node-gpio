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

        static v8::Persistent<v8::FunctionTemplate> ct_constructor;

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
        static v8::Handle<v8::Value> New(const v8::Arguments &args);
        static void Init(v8::Handle<v8::Object> exports);
        static v8::Handle<v8::Value> Open(const v8::Arguments &args);
        static v8::Handle<v8::Value> Close(const v8::Arguments &args);
        static v8::Handle<v8::Value> GetChargeCycle(const v8::Arguments &args);
        static v8::Handle<v8::Value> GetSample(const v8::Arguments &args);

        // v8 exposed properties
        static v8::Handle<v8::Value> GetTimeout(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static void SetTimeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> GetThreshold(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static void SetThreshold(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
};

#endif
