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

#import "NUIProxy.h"
#import "NUIView.h"
#import "nui_view.h"

namespace nui {
    class MacView : public View {
    public:
        MacView(const v8::Local<v8::Object> &handle);
        virtual ~MacView();
        virtual void *GetImpl();
        virtual void Load(const char *path);
        
    protected:
        NUIView *m_impl;
        int m_ref;
    };
    
    MacView::MacView(const v8::Local<v8::Object> &handle): View(handle) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        this->m_impl = [(NUIView *)[NUIProxy allocWithName:@"NUIView"] initWithHandle:this];
        this->m_ref = 0;
        
        [pool release];
    }
    
    MacView::~MacView() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        [this->m_impl release];
        [pool release];
    }
    
    void *MacView::GetImpl() {
        return this->m_impl;
    }
    
    void MacView::Load(const char *str) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSString *path = [[NSString alloc] initWithCString:str encoding:NSUTF8StringEncoding];
        
        [this->m_impl loadFileAtPath:path];
        [path release];
        [pool release];
    }
    
    View *View::CreateNative(const v8::Local<v8::Object> &handle, int x, int y, int width, int height) {
        return new MacView::MacView(handle);
    }
}

@implementation NUIView

- (id)initWithHandle:(void *)handle
{
    self = [self initWithFrame:NSZeroRect frameName:nil groupName:nil];
    
    if(self) {
        self->m_handle = handle;
    }
    
    return self;
}

- (void)loadFileAtPath:(NSString *)path
{
    [[self mainFrame] loadHTMLString:@"<html><body><h1>Hello World</h1></body></html>" baseURL:nil];
    //[[self mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL fileURLWithPath:path]]];
}

@end
