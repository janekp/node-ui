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

#import "NUIApplication.h"
#import "nui_application.h"

namespace nui {
    static Application *_application = NULL;
    
    Application *Application::SharedInstance() {
        if(!_application) {
            _application = new Application();
        }
        
        return _application;
    }
}

#pragma mark - 

@implementation NUIApplication

#pragma mark NSApplicationDelegate

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return NO;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSRunLoop *loop = [NSRunLoop mainRunLoop];
    
    // Block execution until JS code is done
    while(!nui::Ready) {
        NSDate *date = [[NSDate alloc] initWithTimeIntervalSinceNow:0.02];
        
        [loop runMode:NSDefaultRunLoopMode beforeDate:date];
        [date release];
    }
    
    nui::Application::SharedInstance()->Emit("ready");
    
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    nui::Application::SharedInstance()->Emit("activate");
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
    nui::Application::SharedInstance()->Emit("deactivate");
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    NSRunLoop *loop = [NSRunLoop mainRunLoop];
    int state = 1;
    
    nui::Application::SharedInstance()->Emit("close", &state);
    
    // Block execution until JS code is done
    while(state) {
        NSDate *date = [[NSDate alloc] initWithTimeIntervalSinceNow:0.02];
        
        [loop runMode:NSDefaultRunLoopMode beforeDate:date];
        [date release];
    }
}

#pragma mark NSObject

- (id)init
{
    self = [super init];
    
    if(self) {
        [self setDelegate:self];
    }
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

@end
