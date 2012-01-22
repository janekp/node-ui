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

#include <dirent.h>
#include <sys/fcntl.h>
#include "nui_io.h"
#include "nui_resources.h"

namespace nui {
    static char path_separator = '/';
    
    static int ScanDirectory(struct dirent *entry) {
        return (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) ? false : true;
    }
    
    std::string IO::CreateDirectory(const std::string &path) {
        return (mkdir(path.c_str(), 0755) == 0) ? path : std::string("");
    }
    
    int IO::CreateLink(const std::string &path, const std::string &to) {
        return (link(to.c_str(), path.c_str()) == 0) ? 1 : 0;
    }
    
    std::string IO::CreateTemporary() {
        std::string root = IO::CombinePath(IO::CombinePath("", "tmp"), "node-ui");
        
        if(IO::Exists(root) || !IO::CreateDirectory(root).empty()) {
            std::string path = IO::CombinePath(root, "XXXXXXXXXX");
            
            if(!path.empty()) {
                const char *cstr = path.c_str();
                size_t length = strlen(cstr);
                char buffer[length + 1];
                
                path.copy(&(buffer[0]), length);
                buffer[length] = 0x00;
                
                if(mktemp(&(buffer[0])) && mkdir(&(buffer[0]), 0755) == 0) {
                    return std::string(&(buffer[0]));
                }
            }
        }
        
        return std::string("");
    }
    
    std::string IO::CreateResource(const std::string &path, const std::string &name) {
        const char *cstr = name.c_str();
        
        for(int i = 0; _resources[i].name != NULL; i++) {
            Resource resource = _resources[i];
            
            if(strcmp(resource.name, cstr) == 0) {
                mode_t mask = umask(0);
                int fp = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                
                umask(mask);
                
                if(fp) {
                    write(fp, resource.data, resource.length);
                    close(fp);
                    
                    return path;
                }
            }
        }
        
        return std::string("");
    }
    
    std::string IO::CombinePath(const std::string &path, const std::string &name) {
        std::string result;
        
        if(!name.empty() && name.at(0) == path_separator) {
            return name;
        }
        
        if(path.empty()) {
            result.append(&path_separator, 1);
        } else {
            result.append(path);
        }
        
        if(result.empty() || result.at(result.length() - 1) != path_separator) {
            result.append(&path_separator, 1);
        }
        
        result.append(name);
        
        return result;
    }
    
    int IO::DeleteDirectory(const std::string &path) {
        int result = 0;
        struct stat s;
        
        if(!path.empty() && stat(path.c_str(), &s) == 0) {
            result = 1;
            
            if(S_ISDIR(s.st_mode)) {
                struct dirent **entries = NULL;
                int c = scandir(path.c_str(), &entries, &ScanDirectory, NULL);
                size_t p = strlen(path.c_str());
                int i;
                
                for(i = 0; i < c; i++) {
                    struct dirent *entry = entries[i];
                    size_t e = strlen(entry->d_name);
                    char b[p + e + 2];
                    
                    memcpy(&(b[0]), path.c_str(), p);
                    memcpy(&(b[p]), &path_separator, 1);
                    memcpy(&(b[p + 1]), entry->d_name, e);
                    b[p + e + 1] = 0x00;
                    
                    if(stat(&(b[0]), &s) == 0) {
                        if(S_ISDIR(s.st_mode)) {
                            result = IO::DeleteDirectory(&(b[0]));
                        } else {
                            if(remove(&(b[0])) != 0) {
                                result = 0;
                            }
                        }
                    } else {
                        result = 0;
                    }
                    
                    free(entry);
                }
                
                free(entries);
            }
            
            if(remove(path.c_str()) != 0) {
                result = 0;
            }
        }
        
        return result;
    }
    
    std::string IO::DirectoryPath(const std::string &path) {
        for(size_t i = path.length(); i > 0; i--) {
            if(path.at(i - 1) == path_separator) {
                return path.substr(0, i - 1);
            }
        }
        
        return std::string("");
    }
    
    int IO::Exists(const std::string &path) {
        struct stat s;
        
        return (stat(path.c_str(), &s) == 0) ? 1 : 0;
    }
    
    int IO::Exists(const std::string &path, size_t *length) {
        struct stat s;
        
        if(stat(path.c_str(), &s) == 0) {
            if(length) *length = s.st_size;
            
            return 1;
        }
        
        return 0;
    }
}