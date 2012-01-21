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

#ifndef nui_events_h
#define nui_events_h

#include "nui.h"

namespace nui {
    class EventEmitter;
    
    typedef void (*EmitCallback)(EventEmitter *emitter, void *context, const v8::Persistent<v8::Function> &function);
    
    class EventEmitter: public node::ObjectWrap {
    public:
        void Emit(const std::string &name);
        void Emit(const std::string &name, int *state);
        void Emit(const std::string &name, EmitCallback cb, void *context);
        void Emit(EmitCallback cb, void *context);
        virtual ~EventEmitter();
        
        static void Register();
        static void Initialize(const v8::Handle<v8::ObjectTemplate> &target);
        
        static v8::Handle<v8::Value> AddListener(const v8::Arguments &args);
        static v8::Handle<v8::Value> RemoveListener(const v8::Arguments &args);
        static v8::Handle<v8::Value> RemoveAllListeners(const v8::Arguments &args);
        static v8::Handle<v8::Value> Listeners(const v8::Arguments &args);
        
        std::map<std::string, std::vector<v8::Persistent<v8::Function> > > m_events;
    };
}

#endif
