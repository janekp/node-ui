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

#ifndef nui_view_h
#define nui_view_h

#include "nui_events.h"

namespace nui {
    class View: public EventEmitter {
    public:
        static v8::Persistent<v8::FunctionTemplate> constructor;
        static void Initialize(v8::Handle<v8::Object> target);
        
        static v8::Handle<v8::Value> New(const v8::Arguments &args);
        static v8::Handle<v8::Value> Exec(const v8::Arguments &args);
        static v8::Handle<v8::Value> Load(const v8::Arguments &args);
        
        static View *CreateNative(const v8::Local<v8::Object> &handle, int x, int y, int width, int height);
        
        View(const v8::Local<v8::Object> &handle);
        virtual ~View();
        virtual void *GetImpl() = 0;
        virtual void Exec(const char *str, v8::Handle<v8::Value> func) = 0;
        virtual void Load(const char *path) = 0;
        virtual void Load(const void *data, int length) = 0;
    };
}

#endif
