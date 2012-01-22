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

#include "nui_asset.h"
#include "nui_io.h"
#include <node_buffer.h>
#include <sys/fcntl.h>

namespace nui {
    static const Resource *_assets = NULL;
    static const char *_assetsPath = NULL;
    
    const Resource *Asset::GetResource(const char *cstr) {
        if(_assets != NULL) {
            for(int i = 0; _assets[i].name != NULL; i++) {
                const Resource &asset = _assets[i];
                
                if(strcmp(asset.name, cstr) == 0) {
                    return &asset;
                }
            }
        }
        
        return NULL;
    }
    
    const char *Asset::GetDirectory() {
        return _assetsPath;
    }
    
    AssetEntry Asset::GetEntry(const char *path) {
        AssetEntry entry;
        
        entry.resource = Asset::GetResource(path);
        entry.path = NULL;
        entry.length = 0;
        
        if(entry.resource) {
            entry.length = entry.resource->length;
        } else {
            if(IO::Exists(path, &(entry.length))) {
                entry.path = path;
            }
        }
        
        return entry;
    }
    
    void Asset::Initialize(v8::Handle<v8::Object> target) {
        v8::HandleScope scope;
        
        // Register
        target->Set(v8::String::NewSymbol("Asset"), Asset::Template()->GetFunction());
    }
    
    void Asset::Register(const char *path, const Resource *assets) {
        free((void *)_assetsPath);
        _assetsPath = (path != NULL) ? strcpy((char *)malloc(sizeof(char) * (strlen(path) + 1)), path) : NULL;
        _assets = assets;
    }
    
    v8::Persistent<v8::FunctionTemplate> Asset::Template() {
        static v8::Persistent<v8::FunctionTemplate> constructor;
        
        if(constructor.IsEmpty()) {
            constructor = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(Asset::New));
            constructor->InstanceTemplate()->SetInternalFieldCount(1);
            constructor->SetClassName(v8::String::NewSymbol("Asset"));
            
            // Prototype
            v8::Local<v8::ObjectTemplate> proto = constructor->PrototypeTemplate();
            proto->Set(v8::String::NewSymbol("exists"), v8::FunctionTemplate::New(Asset::Exists)->GetFunction());
            proto->Set(v8::String::NewSymbol("toBuffer"), v8::FunctionTemplate::New(Asset::Exists)->GetFunction());
        }
        
        return constructor;
    }
    
    v8::Handle<v8::Value> Asset::New(const v8::Arguments &args) {
        v8::HandleScope scope;
        
        new Asset(args.This(), args[0]);
        
        return args.This();
    }
    
    v8::Handle<v8::Value> Asset::Exists(const v8::Arguments &args) {
        v8::HandleScope scope;
        Asset *asset = node::ObjectWrap::Unwrap<Asset>(args.This());
        AssetEntry entry = asset->GetEntry();
        
        return scope.Close(v8::Boolean::New((entry.path != NULL || entry.resource != NULL) ? true : false));
    }
    
    v8::Handle<v8::Value> Asset::GetLength(v8::Local<v8::String> prop, const v8::AccessorInfo &info) {
        v8::HandleScope scope;
        Asset *asset = node::ObjectWrap::Unwrap<Asset>(info.This());
        
        return scope.Close(v8::Integer::New((int)asset->GetEntry().length));
    }
    
    v8::Handle<v8::Value> Asset::ToBuffer(const v8::Arguments &args) {
        v8::HandleScope scope;
        Asset *asset = node::ObjectWrap::Unwrap<Asset>(args.This());
        AssetEntry entry = asset->GetEntry();
        node::Buffer *buffer = node::Buffer::New(entry.length);
        
        if(entry.resource != NULL) {
            memcpy(node::Buffer::Data(buffer), entry.resource->data, entry.resource->length);
        } else if(entry.path != NULL) {
            int fp = open(entry.path, O_RDONLY);
			
			if(fp) {
				read(fp, node::Buffer::Data(buffer), entry.resource->length);
				close(fp);
			} else {
                buffer = node::Buffer::New(0);
            }
        }
        
        return scope.Close(buffer->handle_);
    }
    
    Asset::Asset(const v8::Local<v8::Object> &handle, const v8::Local<v8::Value> &path) {
        this->Wrap(handle);
        this->m_entry.path = NULL;
        this->m_entry.resource = NULL;
        this->m_entry.length = 0;
        
        if(path->IsString()) {
            v8::Local<v8::String> str = path->ToString();
            int length = str->Utf8Length();
            char buffer[length + 1];
            
            str->WriteUtf8(&(buffer[0]), length + 1);
            
            if((this->m_entry.resource = Asset::GetResource(&(buffer[0]))) != NULL) {
                this->m_entry.length = this->m_entry.resource->length;
            } else {
                if(IO::Exists(&(buffer[0]), &(this->m_entry.length))) {
                    this->m_entry.path = (char *)malloc(sizeof(char) * (length + 1));
                    strncpy((char *)this->m_entry.path, &(buffer[0]), length + 1);
                }
            }
        }
    }
    
    Asset::~Asset() {
        free((void *)this->m_entry.path);
    }
    
    AssetEntry Asset::GetEntry() {
        return this->m_entry;
    }
}