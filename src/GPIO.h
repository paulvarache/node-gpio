#ifndef GPIO_H
#define GPIO_H

#include <string>
#include <node.h>


class GPIO : public node::ObjectWrap
{
    protected:
        static v8::Persistent<v8::FunctionTemplate> constructor;
        int write(int);
        int read();

    public:
        std::string pin_num;
        bool opened;
        std::string mode;
        static std::string IN;
        static std::string OUT;
        static int HIGH;
        static int LOW;
        static bool debug;
        explicit GPIO(std::string num = "4");
        ~GPIO();
        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static void Init(v8::Handle<v8::Object> target);
        static v8::Handle<v8::Value> GetNum(v8::Local<v8::String> property, const v8::AccessorInfo& info);
        static v8::Handle<v8::Value> Open(const v8::Arguments& args);
        static v8::Handle<v8::Value> Close(const v8::Arguments& args);
        static v8::Handle<v8::Value> SetMode(const v8::Arguments& args);
        static v8::Handle<v8::Value> Write(const v8::Arguments& args);
        static v8::Handle<v8::Value> Read(const v8::Arguments& args);
        static v8::Handle<v8::Value> Toggle(const v8::Arguments& args);
        int WriteValue(std::string path, std::string val);
        int ReadValue(std::string path);
        void log(std::string msg);
};

#endif
