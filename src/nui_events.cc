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

namespace nui {
    typedef struct _Event {
        EmitCallback call;
        void *context;
        struct _Event *next;
        EventEmitter *emitter;
        char *name;
    } Event;
    
    static Event *events = NULL;
    static pthread_mutex_t events_lock = PTHREAD_MUTEX_INITIALIZER;
    static uv_timer_t events_timer;
    
    static void EmitStateCallback(EventEmitter *emitter, void *context, const v8::Persistent<v8::Function> &function) {
        if(emitter) {
            function->Call(emitter->handle_, 0, NULL);
        } else {
            *((int *)context) = 0x00;
        }
    }
    
    static void CheckEmitters(uv_timer_t *handle, int status) {
        if(events != NULL) {
            Event *e, *n;
            
            pthread_mutex_lock(&events_lock);
            e = events;
            events = NULL;
            pthread_mutex_unlock(&events_lock);
            
            while(e) {
                v8::TryCatch try_catch;
                std::string key(e->name);
                n = e->next;
                
                if(e->emitter->m_events.find(key) != e->emitter->m_events.end()) {
                    std::vector<v8::Persistent<v8::Function> > events = e->emitter->m_events[key];
                    
                    for(std::vector<v8::Persistent<v8::Function> >::iterator it = events.begin(); it != events.end(); ++it) {
                        v8::HandleScope scope;
                        
                        if(e->call) {
                            (e->call)(e->emitter, e->context, *it);
                        } else {
                            (*it)->Call(e->emitter->handle_, 0, NULL);
                        }
                        
                        if(try_catch.HasCaught()) {
                            node::FatalException(try_catch);
                            break;
                        }
                    }
                }
                
                if(e->call) (e->call)(NULL, e->context, v8::Persistent<v8::Function>());
                free(e);
                e = n;
            }
        }
    }
    
    EventEmitter::~EventEmitter() {
        // TODO: 
    }
    
    void EventEmitter::Emit(const std::string &name) {
        this->Emit(name, NULL, NULL);
    }
    
    void EventEmitter::Emit(const std::string &name, int *state) {
        this->Emit(name, &EmitStateCallback, state);
    }
    
    void EventEmitter::Emit(const std::string &name, EmitCallback cb, void *context) {
        const char *chars = name.c_str();
        size_t length = strlen(chars);
        Event *e = (Event *)malloc(sizeof(struct _Event) + (length + 1) * sizeof(char));
        
        e->call = cb;
        e->context = context;
        e->next = NULL;
        e->emitter = this;
        e->name = (char *)e + sizeof(struct _Event);
        strcpy(e->name, chars);
        
        pthread_mutex_lock(&events_lock);
        
        if(events) {
            Event *n = events;
            
            while(n->next) {
                n = n->next;
            }
            
            n->next = e;
        } else {
            events = e;
        }
        
        pthread_mutex_unlock(&events_lock);
    }
    
    void EventEmitter::Register() {
        uv_timer_init(uv_default_loop(), &events_timer);
        uv_timer_start(&events_timer, CheckEmitters, 10, 10); // 10ms
    }
    
    
    void EventEmitter::Initialize(const v8::Handle<v8::ObjectTemplate> &target) {
        target->Set(v8::String::NewSymbol("addListener"), v8::FunctionTemplate::New(EventEmitter::AddListener)->GetFunction());
        target->Set(v8::String::NewSymbol("on"), v8::FunctionTemplate::New(EventEmitter::AddListener)->GetFunction());
        target->Set(v8::String::NewSymbol("removeListener"), v8::FunctionTemplate::New(EventEmitter::RemoveListener)->GetFunction());
        target->Set(v8::String::NewSymbol("removeAllListeners"), v8::FunctionTemplate::New(EventEmitter::RemoveAllListeners)->GetFunction());
        target->Set(v8::String::NewSymbol("listeners"), v8::FunctionTemplate::New(EventEmitter::Listeners)->GetFunction());
    }
    
    v8::Handle<v8::Value> EventEmitter::AddListener(const v8::Arguments &args) {
        v8::HandleScope scope;
        
        if(args.Length() == 2) {
            v8::String::Utf8Value name(args[0]);
            v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[1]);
            std::string key = std::string(*name, name.length());
            EventEmitter *emitter = node::ObjectWrap::Unwrap<EventEmitter>(args.This());
            
            if(emitter->m_events.find(key) == emitter->m_events.end()) {
                emitter->m_events[key] = std::vector<v8::Persistent<v8::Function> >();
            }
            
            emitter->m_events[key].push_back(v8::Persistent<v8::Function>::New(cb));
            ev_ref(EV_DEFAULT_UC);
        }
        
        return scope.Close(v8::Undefined());
    }
    
    v8::Handle<v8::Value> EventEmitter::RemoveListener(const v8::Arguments &args) {
        v8::HandleScope scope;
        
        // TODO: 
        
        return scope.Close(v8::Undefined());
    }
    
    v8::Handle<v8::Value> EventEmitter::RemoveAllListeners(const v8::Arguments &args) {
        v8::HandleScope scope;
        
        // TODO: 
        
        return scope.Close(v8::Undefined());
    }
    
    v8::Handle<v8::Value> EventEmitter::Listeners(const v8::Arguments &args) {
        v8::HandleScope scope;
        
        // TODO: 
        
        return scope.Close(v8::Undefined());
    }
}
