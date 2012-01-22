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

#ifndef nui_asset_h
#define nui_asset_h

#include "nui.h"

namespace nui {
    typedef struct _AssetEntry {
        const char *path;
        const Resource *resource;
        size_t length;
    } AssetEntry;
    
    class Asset: public node::ObjectWrap {
    public:
        static const char *GetDirectory();
        static AssetEntry GetEntry(const char *path);
        static void Initialize(v8::Handle<v8::Object> target);
        static void Register(const char *path, const Resource *assets);
        
        AssetEntry GetEntry();
    
    private:
        static v8::Persistent<v8::FunctionTemplate> Template();
        static v8::Handle<v8::Value> New(const v8::Arguments &args);
        static v8::Handle<v8::Value> Exists(const v8::Arguments &args);
        static v8::Handle<v8::Value> GetLength(v8::Local<v8::String> prop, const v8::AccessorInfo &info);
        static v8::Handle<v8::Value> ToBuffer(const v8::Arguments &args);
        
        static const Resource *GetResource(const char *name);
        
        Asset(const v8::Local<v8::Object> &handle, const v8::Local<v8::Value> &path);
        virtual ~Asset();
        
        AssetEntry m_entry;
    };
}

#endif
