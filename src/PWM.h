#ifndef PWM_H
#define PWM_H

#include <string>
#include <node.h>
#include "GPIO.h"
#include <thread>


class PWM : public GPIO
{
    static v8::Persistent<v8::FunctionTemplate> pwm_constructor;
    void start();
    void stop();
    void run();
    std::thread thread;

    public:
        int freq;
        int dc;
        bool running;
        explicit PWM(std::string num = "4");
        ~PWM();
        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static void Init(v8::Handle<v8::Object> target);
        static v8::Handle<v8::Value> Start(const v8::Arguments& args);
        static v8::Handle<v8::Value> Stop(const v8::Arguments& args);
        static v8::Handle<v8::Value> GetFreq(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static void SetFreq(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> GetDC(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static void SetDC(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
};

#endif
