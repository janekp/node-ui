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

#include <fstream>
#include "nui.h"
#include "nui_asset.h"
#include "nui_io.h"
#include "nui_package.h"
#include "nui_utils.h"

extern "C" {
    #include <Foundation/Foundation.h>
    #include <AppKit/AppKit.h>
}

namespace nui {
    int Main(int argc, char *argv[]) {
        std::string dir = Utils::GetApplicationDirectory(argc, argv);
        
        // The app is being launched from GUI or execv()
        if(argc > 0 && strstr(argv[0], ".app/Contents/MacOS")) {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            int result;
            
            // Set the root directory for all the assets
            if(!dir.empty()) {
                Asset::Register(dir.c_str(), NULL);
            }
            
            // Execute all the scripts, setup async event handlers
            Execute(argc, argv);
            
            // Execute the main run loop for UI
            result = NSApplicationMain(1, (const char **)argv);
            [pool release];
            
            return result;
        // The app is being launched from CLI (so lets generate an .app)
        } else {
            Package package((!dir.empty()) ? IO::CombinePath(dir, std::string("package.json")) : std::string());
            std::string tmp = IO::CreateTemporary();
            std::string app = IO::CreateDirectory(IO::CombinePath(tmp, package.GetExecutableName() + std::string(".app")));
            std::string contents = IO::CreateDirectory(IO::CombinePath(app, std::string("Contents")));
            std::string macos = IO::CreateDirectory(IO::CombinePath(contents, std::string("MacOS")));
            std::string resources = IO::CreateDirectory(IO::CombinePath(contents, std::string("Resources")));
            std::string infoPlist = IO::CombinePath(contents, std::string("Info.plist"));
            std::string binary = IO::CombinePath(macos, std::string("node-ui"));
            std::ofstream file;
            pid_t pid;
            int fd[2];
            
            IO::CreateResource(IO::CombinePath(resources, std::string("MainMenu.nib")), std::string("MainMenu.nib"));
            IO::CreateResource(IO::CombinePath(contents, std::string("PkgInfo")), std::string("PkgInfo"));
            
            file.open(infoPlist.c_str());
            file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
            file << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">";
            file << "<plist version=\"1.0\">";
            file << "<dict>";
            file << "<key>CFBundleDevelopmentRegion</key>";
            file << "<string>English</string>";
            file << "<key>CFBundleExecutable</key>";
            file << "<string>node-ui</string>";
            file << "<key>CFBundleIconFile</key>";
            file << "<string>Icon</string>";
            file << "<key>CFBundleIdentifier</key>";
            file << "<string>com.node-ui.simulator</string>";
            file << "<key>CFBundleInfoDictionaryVersion</key>";
            file << "<string>6.0</string>";
            file << "<key>CFBundleName</key>";
            file << "<string>" + package.GetName() + "</string>";
            file << "<key>CFBundlePackageType</key>";
            file << "<string>APPL</string>";
            file << "<key>CFBundleSignature</key>";
            file << "<string>????</string>";
            file << "<key>CFBundleVersion</key>";
            file << "<string>1.0</string>";
            file << "<key>LSHasLocalizedDisplayName</key>";
            file << "<true/>";
            file << "<key>NSMainNibFile</key>";
            file << "<string>MainMenu</string>";
            file << "<key>NSPrincipalClass</key>";
            file << "<string>NUIApplication</string>";
            file << "</dict>";
            file << "</plist>";
            file.close();
            
            if(argc > 0) {
                IO::CreateLink(binary, std::string(argv[0]));
            }
            
            // Create a pipe to monitor script status and fork a clean-up process.
            pipe(fd);
            pid = fork();
            
            // Clean-up crew
            if(pid == 0) {
                char buffer;
                
                // Close output and wait until input is closed too.
                close(fd[1]);
                read(fd[0], &buffer, 1);
                
                // Finally delete the autogenerated .app.
                IO::DeleteDirectory(tmp);
                
                return EXIT_SUCCESS;
            // Error
            } else if(pid < 0) {
                perror("fork() error");
            // Lets relaunch the process with GUI
            } else {
                const char *path = binary.c_str();
                char *argvv[argc + 1];
                
                argvv[0] = (char *)path;
                
                for(int argi = 1; argi < argc; argi++) {
                    argvv[argi] = argv[argi];
                }
                
                argvv[argc] = NULL;
                
                close(fd[0]);
                
                if(execv(path, argvv) < 0) {
                    perror("execv() error");
                }
            }
            
            return EXIT_FAILURE;
        }        
        
        return EXIT_SUCCESS;
    }
}
