#ifndef PWM_H
#define PWM_H

#include <string>
#include <node.h>
#include "GPIO.h"


class PWM : public node::ObjectWrap
{
    static v8::Persistent<v8::Function> pwm_constructor;

    public:
        int freq;
        explicit PWM(std::string num = "4");
        ~PWM();
        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static void Init(v8::Handle<v8::Object> target);
};

#endif
