// Copyright Janek Priimann.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef nui_window_h
#define nui_window_h

#include "nui_events.h"
#include "nui_view.h"

namespace nui {
    class Window : public EventEmitter {
    public:
        static v8::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        
        static v8::Handle<v8::Value> New(const v8::Arguments &args);
        static v8::Handle<v8::Value> GetVisible(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
        static v8::Handle<v8::Value> GetView(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
        static void SetView(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info);
        static v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
        static void SetWidth(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info);
        static v8::Handle<v8::Value> GetHeight(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
        static void SetHeight(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info);
        static v8::Handle<v8::Value> GetTitle(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
        static void SetTitle(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info);
        static v8::Handle<v8::Value> Show(const v8::Arguments &args);
        static v8::Handle<v8::Value> Hide(const v8::Arguments &args);
        
        static Window *CreateNative(const v8::Local<v8::Object> &handle, int width, int height, const char *title);
        
        static std::vector<Window *> windows;
        
        Window(const v8::Local<v8::Object> &handle);
        virtual ~Window();
        virtual View *GetView() = 0;
        virtual void Show() = 0;
        virtual void Hide() = 0;
        virtual int IsVisible() = 0;
        virtual int GetWidth() = 0;
        virtual void SetWidth(int width) = 0;
        virtual int GetHeight() = 0;
        virtual void SetHeight(int height) = 0;
        virtual std::string GetTitle() = 0;
        virtual void SetTitle(std::string title) = 0;
    };
}

#endif
