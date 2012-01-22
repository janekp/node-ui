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

#include "nui_buffer.h"
#include "nui_view.h"

namespace nui {
    v8::Persistent<v8::FunctionTemplate> View::constructor;
    
    void View::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        // Constructor
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(View::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(v8::String::NewSymbol("View"));
        
        // Prototype
        v8::Local<v8::ObjectTemplate> proto = constructor->PrototypeTemplate();
        EventEmitter::Initialize(proto);
        proto->Set(v8::String::NewSymbol("exec"), v8::FunctionTemplate::New(View::Exec)->GetFunction());
        proto->Set(v8::String::NewSymbol("load"), v8::FunctionTemplate::New(View::Load)->GetFunction());
        
        // Register
        target->Set(v8::String::NewSymbol("View"), constructor->GetFunction());
    }
    
    v8::Handle<v8::Value> View::New(const v8::Arguments &args) {
        int width = 0, height = 0;
        v8::HandleScope scope;
        View *view;
        
        if(args[0]->IsNumber()) width = args[0]->Uint32Value();
        if(args[1]->IsNumber()) height = args[1]->Uint32Value();
        
        view = View::CreateNative(args.This(), 0, 0, width, height);
        
        return args.This();
    }
    
    v8::Handle<v8::Value> View::Exec(const v8::Arguments &args) {
        v8::HandleScope scope;
        View *view = node::ObjectWrap::Unwrap<View>(args.This());
        
        if(args[0]->IsString()) {
            v8::Local<v8::String> str = args[0]->ToString();
            int length = str->Utf8Length();
            char buffer[length + 1];
            
            str->WriteUtf8(&(buffer[0]), length + 1);
            view->Exec(&(buffer[0]), args[1]);
        }
        
        return scope.Close(v8::Undefined());
    }
    
    v8::Handle<v8::Value> View::Load(const v8::Arguments &args) {
        v8::HandleScope scope;
        View *view = node::ObjectWrap::Unwrap<View>(args.This());
        
        if(args[0]->IsString()) {
            v8::Local<v8::String> str = args[0]->ToString();
            int length = str->Utf8Length();
            
            if(length > 0) {
                char buffer[length + 1];
                
                str->WriteUtf8(&(buffer[0]), length + 1);
                view->Load(&(buffer[0]));
            }
        } else if(args[0]->IsFunction()) {
            v8::TryCatch try_catch;
            v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[0]);
            Buffer *buffer = Buffer::Create();
            v8::Handle<v8::Value> args[1] = { buffer->handle_ };
            v8::Persistent<v8::Object> handle = buffer->handle_;
            
            cb->Call(view->handle_, 1, args);
            
            if(try_catch.HasCaught()) {
                node::FatalException(try_catch);
                return scope.Close(v8::Undefined());
            }
            
            view->Load(buffer->GetData(), buffer->GetLength());
            handle.Dispose();
            handle.Clear();
        }
        
        return scope.Close(v8::Undefined());
    }
    
    View::View(const v8::Local<v8::Object> &handle): EventEmitter() {
        this->Wrap(handle);
        //v8::V8::AdjustAmountOfExternalAllocatedMemory(300 * 1024 * 1024);
    }
    
    View::~View() {
        //v8::V8::AdjustAmountOfExternalAllocatedMemory(-300 * 1024 * 1024);
    }
}