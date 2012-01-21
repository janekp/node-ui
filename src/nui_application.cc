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

#include "nui_application.h"
#include "nui_window.h"

namespace nui {
    Application::Application() {
    }
    
    Application::~Application() {
    }
    
    void Application::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        v8::Handle<v8::ObjectTemplate> templ = v8::ObjectTemplate::New();
        
        templ->SetInternalFieldCount(1);
        EventEmitter::Initialize(templ);
        templ->SetAccessor(v8::String::NewSymbol("session"), Application::GetSession, NULL);
        templ->SetAccessor(v8::String::NewSymbol("windows"), Application::GetWindows, NULL);
        
        v8::Local<v8::Object> application = templ->NewInstance();
        Application::SharedInstance()->Wrap(application);
        
        target->Set(v8::String::NewSymbol("application"), application);
    }
    
    v8::Handle<v8::Value> Application::GetSession(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        
        // TODO: 
        
        return scope.Close(v8::String::New("TDB"));
    }
    
    v8::Handle<v8::Value> Application::GetWindows(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        v8::Local<v8::Array> result = v8::Array::New();
        int index = 0;
        
        for(std::vector<Window *>::iterator i = Window::windows.begin(); i < Window::windows.end(); i++) {
            if((*i)->IsVisible()) {
                result->Set(v8::Number::New(index++), (*i)->handle_);
            }
        }
        
        return scope.Close(result);
    }
}