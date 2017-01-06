#ifndef PWM_H
#define PWM_H

#include <string>
#include <node.h>
#include "GPIO.h"
#include <thread>

class PWM : public GPIO
{
    static v8::Persistent<v8::FunctionTemplate> constructor;
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

        int setMode(std::string);

        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Init(v8::Local<v8::Object> target);
        static void Start(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Stop(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetFreq(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
        static void SetFreq(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        static void GetDC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
        static void SetDC(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
};

#endif
