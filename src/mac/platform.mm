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

#include "nui_events.h"
#include <Foundation/Foundation.h>

namespace nui {
    static void EmitNativeCallback(EventEmitter *emitter, void *context, const v8::Persistent<v8::Function> &function) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSString *obj = (NSString *)context;
        
        if(emitter) {
            v8::Local<v8::String> str = v8::String::New([obj UTF8String]);
            v8::Handle<v8::Value> args[1] = { str };
            
            function->Call(emitter->handle_, 1, args);
        } else {
            [obj release];
        }
        
        [pool release];
    }
    
    void EventEmitter::Emit(const std::string &name, void *nativePtr) {
        this->Emit(name, EmitNativeCallback, [(id)nativePtr retain]);
    }
}