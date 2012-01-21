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

#include "nui_window.h"

namespace nui {
    v8::Persistent<v8::FunctionTemplate> Window::constructor;
    
    static void InitializeCommon(const v8::Local<v8::ObjectTemplate> &proto) {
        EventEmitter::Initialize(proto);
        proto->SetAccessor(v8::String::NewSymbol("view"), Window::GetView, Window::SetView);
        proto->SetAccessor(v8::String::NewSymbol("width"), Window::GetWidth, Window::SetWidth);
        proto->SetAccessor(v8::String::NewSymbol("height"), Window::GetHeight, Window::SetHeight);
        proto->SetAccessor(v8::String::NewSymbol("title"), Window::GetTitle, Window::SetTitle);
        proto->SetAccessor(v8::String::NewSymbol("visible"), Window::GetVisible, NULL);
        proto->Set(v8::String::NewSymbol("show"), v8::FunctionTemplate::New(Window::Show)->GetFunction());
        proto->Set(v8::String::NewSymbol("hide"), v8::FunctionTemplate::New(Window::Hide)->GetFunction());
    }
    
    void Window::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        // Constructor
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(Window::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(v8::String::NewSymbol("Window"));
        
        // Prototype
        v8::Local<v8::ObjectTemplate> proto = constructor->PrototypeTemplate();
        InitializeCommon(proto);
        
        // Register
        target->Set(v8::String::NewSymbol("Window"), constructor->GetFunction());
    }
    
    v8::Handle<v8::Value> Window::New(const v8::Arguments &args) {
        int width = 0, height = 0;
        char *title = NULL;
        v8::HandleScope scope;
        Window *window;
        
        if(args[0]->IsNumber()) width = args[0]->Uint32Value();
        if(args[1]->IsNumber()) height = args[1]->Uint32Value();
        if(args[2]->IsString()) {
            v8::Local<v8::String> str = args[2]->ToString();
            int length = str->Utf8Length();
            
            if(length > 0) {
                title = (char *)malloc(sizeof(char) * length + 1);
                str->WriteUtf8(title, length + 1);
            }
        }
        
        window = Window::CreateNative(args.This(), width, height, title);
        if(title) free(title);
        
        return args.This();
    }
    
    v8::Handle<v8::Value> Window::GetVisible(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        Window *window = node::ObjectWrap::Unwrap<Window>(info.This());
        
        return scope.Close(v8::Number::New(window->IsVisible()));
    }
    
    v8::Handle<v8::Value> Window::GetView(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        Window *window = node::ObjectWrap::Unwrap<Window>(info.This());
        View *view = window->GetView();
        
        if(view) {
            return scope.Close(view->handle_);
        }
        
        return scope.Close(v8::Undefined());
    }
    
    void Window::SetView(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info) {
        // TODO: 
    }
    
    v8::Handle<v8::Value> Window::GetWidth(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        
        // TODO:
        
        return scope.Close(v8::Undefined());
    }
    
    void Window::SetWidth(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info) {
        // TODO: 
    }
    
    v8::Handle<v8::Value> Window::GetHeight(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        
        // TODO:
        
        return scope.Close(v8::Undefined());
    }
    
    void Window::SetHeight(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info) {
        // TODO: 
    }
    
    v8::Handle<v8::Value> Window::GetTitle(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        Window *window = node::ObjectWrap::Unwrap<Window>(info.This());
        
        return scope.Close(v8::String::New(window->GetTitle().c_str()));
    }
    
    void Window::SetTitle(v8::Local<v8::String> prop, v8::Local<v8::Value> val, const v8::AccessorInfo &info) {
        if(val->IsString()) {
            v8::HandleScope scope;
            Window *window = node::ObjectWrap::Unwrap<Window>(info.This());
            v8::Local<v8::String> str = val->ToString();
            int length = str->Utf8Length();
            
            if(length > 0) {
                char buffer[length + 1];
                
                str->WriteUtf8(&(buffer[0]), length + 1);
                window->SetTitle(std::string(&(buffer[0])));
            }
        }
    }
    
    v8::Handle<v8::Value> Window::Show(const v8::Arguments &args) {
        v8::HandleScope scope;
        Window *window = node::ObjectWrap::Unwrap<Window>(args.This());
        
        window->Show();
        
        return scope.Close(v8::Undefined());
    }
    
    v8::Handle<v8::Value> Window::Hide(const v8::Arguments &args) {
        v8::HandleScope scope;
        Window *window = node::ObjectWrap::Unwrap<Window>(args.This());
        
        window->Hide();
        
        return scope.Close(v8::Undefined());
    }
    
    std::vector<Window *> Window::windows;
    
    Window::Window(const v8::Local<v8::Object> &handle): EventEmitter() {
        this->Wrap(handle);
        Window::windows.push_back(this);
        // TODO: We should probably adjust memory, but how much?
        //v8::V8::AdjustAmountOfExternalAllocatedMemory(300 * 1024 * 1024);
    }
    
    Window::~Window() {
        //v8::V8::AdjustAmountOfExternalAllocatedMemory(-300 * 1024 * 1024);
        for(std::vector<Window *>::iterator i = Window::windows.begin(); i < Window::windows.end(); i++) {
            if(*i == this) {
                Window::windows.erase(i);
                break;
            }
        }
    }
    
    v8::Persistent<v8::FunctionTemplate> WebWindow::constructor;
    
    void WebWindow::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        // Constructor
        constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(WebWindow::New));
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->SetClassName(v8::String::NewSymbol("WebWindow"));
        
        // Prototype
        v8::Local<v8::ObjectTemplate> proto = constructor->PrototypeTemplate();
        InitializeCommon(proto);
        proto->Set(v8::String::NewSymbol("exec"), v8::FunctionTemplate::New(WebWindow::Exec)->GetFunction());
        proto->Set(v8::String::NewSymbol("load"), v8::FunctionTemplate::New(WebWindow::Load)->GetFunction());
        
        // Register
        target->Set(v8::String::NewSymbol("WebWindow"), constructor->GetFunction());
    }
    
    v8::Handle<v8::Value> WebWindow::New(const v8::Arguments &args) {
        v8::HandleScope scope;
        v8::Local<v8::Object> handle = View::constructor->GetFunction()->NewInstance(0, NULL);
        Window *window;
        
        Window::New(args);
        window = node::ObjectWrap::Unwrap<Window>(args.This());
        window->SetView(node::ObjectWrap::Unwrap<View>(handle));
        
        return args.This();
    }
    
    v8::Handle<v8::Value> WebWindow::Exec(const v8::Arguments &args) {
        v8::HandleScope scope;
        Window *window = node::ObjectWrap::Unwrap<Window>(args.This());
        View *view = window->GetView();
        
        if(view) {
            v8::Handle<v8::Value> exec = view->handle_->GetPrototype()->ToObject()->Get(v8::String::NewSymbol("exec"));
            
            if(exec->IsFunction()) {
                v8::Function *cb = (v8::Function *)(*exec);
                v8::Handle<v8::Value> argv[1] = { args[0] };
                
                return scope.Close(cb->Call(view->handle_, 1, argv));
            }
        }
        
        return scope.Close(v8::Undefined());
    }
    
    v8::Handle<v8::Value> WebWindow::Load(const v8::Arguments &args) {
        v8::HandleScope scope;
        Window *window = node::ObjectWrap::Unwrap<Window>(args.This());
        View *view = window->GetView();
        
        if(view) {
            v8::Handle<v8::Value> load = view->handle_->GetPrototype()->ToObject()->Get(v8::String::NewSymbol("load"));
            
            if(load->IsFunction()) {
                v8::Function *cb = (v8::Function *)(*load);
                v8::Handle<v8::Value> argv[1] = { args[0] };
                
                return scope.Close(cb->Call(view->handle_, 1, argv));
            }
        }
        
        return scope.Close(v8::Undefined());
    }
}