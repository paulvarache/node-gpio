#ifndef GPIO_H
#define GPIO_H

#include <string>
#include <node.h>
#include <node_object_wrap.h>

class GPIO : public node::ObjectWrap
{
    protected:
        static v8::Persistent<v8::FunctionTemplate> constructor;

    public:
        // Constructor destructor
        explicit GPIO(std::string num = "4");
        ~GPIO();

        // Properties
        std::string pin_num;
        bool opened;
        std::string mode;
        static std::string IN;
        static std::string OUT;
        static int HIGH;
        static int LOW;
        static bool debug;

        // Methods
        int open();
        int close();
        int write(int);
        int read();
        virtual int setMode(std::string mode);
        int WriteValue(std::string path, std::string val);
        int ReadValue(std::string path);
        void log(std::string msg);

        // V8 exposed Methods
        static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Init(v8::Local<v8::Object> exports);
        static void GetNum(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
        static void Open(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Close(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void SetMode(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Write(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void Toggle(const v8::FunctionCallbackInfo<v8::Value>& args);
};

#endif
