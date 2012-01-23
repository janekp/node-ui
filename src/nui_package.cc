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

#include <json/reader.h>
#include "nui_io.h"
#include "nui_package.h"

namespace nui {
    Package::Package(const std::string &path) {
        Json::Features features;
        Json::Reader reader(features);
        Json::Value root;
        
        if(reader.parse(IO::ReadFileToString(path), root) && root.type() == Json::objectValue) {
            Json::Value null = Json::Value();
            Json::Value value;
            
            value = root.get("name", null);
            this->m_name = (value.isString()) ? value.asString() : std::string("node-ui");
            
            value = root.get("version", null);
            this->m_version = (value.isString()) ? value.asString() : std::string("0.1");
            
            value = root.get("copyright", null);
            if(value.isString()) this->m_copyright = value.asString();
            
            value = root.get("icons", null);
            
            if(value.isArray()) {
                for(int i = 0, c = value.size(); i < c; i++) {
                    this->m_icons.push_back(value[i].asString());
                }
            } else {
                value = root.get("icon", null);
                
                if(value.isString()) {
                    this->m_icons.push_back(value.asString());
                } else {
                    this->m_icons.push_back(std::string("icon.png"));
                }
            }
        }
    }
    
    const std::string &Package::GetName() {
        return this->m_name;
    }
    
    const std::string &Package::GetCopyright() {
        return this->m_copyright;
    }
    
    const std::string &Package::GetVersion() {
        return this->m_version;
    }
    
    const std::vector<std::string> &Package::GetIcons() {
        return this->m_icons;
    }
}
