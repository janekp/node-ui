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

namespace nui {
    v8::Persistent<v8::FunctionTemplate> Buffer::constructor;
    
    Buffer *Buffer::Create() {
        v8::Local<v8::Object> handle = Buffer::constructor->GetFunction()->NewInstance(0, NULL);
        
        return node::ObjectWrap::Unwrap<Buffer>(handle);
    }
    
    void Buffer::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        // Constructor
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(Buffer::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(v8::String::NewSymbol("Buffer"));
        
        // Prototype
        v8::Local<v8::ObjectTemplate> proto = constructor->PrototypeTemplate();
        proto->Set(v8::String::NewSymbol("write"), v8::FunctionTemplate::New(Buffer::Write)->GetFunction());
    }
    
    v8::Handle<v8::Value> Buffer::New(const v8::Arguments &args) {
        v8::HandleScope scope;
        
        new Buffer(args.This());
        
        return args.This();
    }
    
    v8::Handle<v8::Value> Buffer::Write(const v8::Arguments &args) {
        v8::HandleScope scope;
        v8::Local<v8::String> str = args[0]->ToString();
        int length = str->Utf8Length();
        
        if(length > 0) {
            Buffer *buffer = node::ObjectWrap::Unwrap<Buffer>(args.This());
            
            buffer->m_data = (char *)realloc(buffer->m_data, sizeof(char) * (buffer->m_length + length));
            str->WriteUtf8(buffer->m_data + buffer->m_length);
            buffer->m_length += length;
        }
        
        return scope.Close(v8::Undefined());
    }
    
    Buffer::Buffer(const v8::Local<v8::Object> &handle) {
        this->Wrap(handle);
        this->m_data = NULL;
        this->m_length = 0;
    }
    
    Buffer::~Buffer() {
        free(this->m_data);
    }
}
