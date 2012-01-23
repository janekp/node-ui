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

#include "nui_io.h"
#include "nui_utils.h"

namespace nui {
    std::string Utils::GetApplicationDirectory(int argc, char *argv[]) {
        // This is not nice, but so what atm?
        for(int argi; argi < argc; argi++) {
            const char *x = strstr(argv[argi], ".js");
            
            if(x != NULL && strlen(x) == 3) {
                char buffer[PATH_MAX];
                
                getcwd(&(buffer[0]), PATH_MAX);
                return IO::DirectoryPath(IO::CombinePath(std::string(&(buffer[0])), std::string(argv[argi])));
                break;
            }
        }
        
        return std::string();
    }
    
    std::string Utils::ToString(const v8::Handle<v8::Value> &data) {
        if(data->IsString()) {
            v8::Local<v8::String> str = data->ToString();
            int length = str->Utf8Length();
            
            if(length > 0) {
                char buffer[length + 1];
                
                str->WriteUtf8(&(buffer[0]), length + 1);
                
                return std::string(&(buffer[0]));
            }
        }
        
        return std::string();
    }
}