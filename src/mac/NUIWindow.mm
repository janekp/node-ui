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
#import "NUIWindow.h"
#import "nui_window.h"

namespace nui {
    class MacWindow : public Window {
    public:
        static void CloseCallback(EventEmitter *emitter, void *context, const v8::Persistent<v8::Function> &function);
        
        MacWindow(const v8::Local<v8::Object> &handle, int width, int height, const char *title);
        virtual ~MacWindow();
        virtual void Show();
        virtual void Hide();
        virtual int IsVisible();
        virtual View *GetView();
        virtual void SetView(View *view);
        virtual int GetWidth();
        virtual void SetWidth(int width);
        virtual int GetHeight();
        virtual void SetHeight(int height);
        virtual std::string GetTitle();
        virtual void SetTitle(std::string title);
        
    protected:
        View *m_view;
        NUIWindow *m_impl;
        int m_ref;
    };
    
    MacWindow::MacWindow(const v8::Local<v8::Object> &handle, int width, int height, const char *title): Window(handle) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        this->m_impl = (NUIWindow *)[NUIProxy allocWithClass:[NUIWindow class]];
        this->m_ref = 0;
        this->m_view = NULL;
        
        [this->m_impl initWithHandle:this contentRect:NSMakeRect(0.0F, 0.0F, width, height) styleMask:NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask backing:NSBackingStoreBuffered defer:YES];
        
        if(title) {
            [this->m_impl setTitle:[[[NSString alloc] initWithCString:title encoding:NSUTF8StringEncoding] autorelease]];
        }
        
        [pool release];
    }
    
    MacWindow::~MacWindow() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        [this->m_impl release];
        [pool release];
    }
    
    int MacWindow::IsVisible() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        int result = [this->m_impl isVisible];
        
        [pool release];
        
        return result;
    }
    
    void MacWindow::Show() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        if(![this->m_impl isVisible]) {
            if(this->m_ref == 0) {
                this->m_ref = 1;
                this->Ref();
            }
            
            [this->m_impl orderFront:nil];
        }
        
        [pool release];
    }
    
    void MacWindow::Hide() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        if([this->m_impl isVisible]) {
            [this->m_impl orderOut:nil];
        }
        
        [pool release];
    }
    
    View *MacWindow::GetView() {
        return this->m_view;
    }
    
    void MacWindow::SetView(View *view) {
        if(this->m_view != view) {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            
            this->m_view = view;
            [this->m_impl setContentView:NUIObject(this->m_view->GetImpl())];
            [pool release];
        }
    }
    
    int MacWindow::GetWidth() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        int result = round([this->m_impl frame].size.width);
        
        [pool release];
        
        return result;
    }
    
    void MacWindow::SetWidth(int width) {
        
    }
    
    int MacWindow::GetHeight() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        int result = round([this->m_impl frame].size.height);
        
        [pool release];
        
        return result;
    }
    
    void MacWindow::SetHeight(int height) {
        
    }
    
    std::string MacWindow::GetTitle() {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSString *title = [this->m_impl title];
        std::string result((title) ? title.UTF8String : "");
        
        [pool release];
        
        return result;
    }
    
    void MacWindow::SetTitle(std::string str) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSString *title = [[NSString alloc] initWithCString:str.c_str() encoding:NSUTF8StringEncoding];
        
        [this->m_impl setTitle:title];
        [title release];
        [pool release];
    }
    
    Window *Window::CreateNative(const v8::Local<v8::Object> &handle, int width, int height, const char *title) {
        return new MacWindow::MacWindow(handle, width, height, title);
    }
    
    void MacWindow::CloseCallback(EventEmitter *emitter, void *context, const v8::Persistent<v8::Function> &function) {
        if(emitter) {
            function->Call(emitter->handle_, 0, NULL);
        } else {
            MacWindow *window = (MacWindow *)context;
            
            if(window->m_ref != 0) {
                window->m_ref = 0;
                window->Unref();
            }
        }
    }
}

#pragma mark - 

@implementation NUIWindow

- (id)initWithHandle:(void *)handle contentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag
{
    self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag];
    
    if(self) {
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        
        [center addObserver:self selector:@selector(didMiniaturize:) name:NSWindowDidMiniaturizeNotification object:self];
        [center addObserver:self selector:@selector(didDeminiaturize:) name:NSWindowDidDeminiaturizeNotification object:self];
        [center addObserver:self selector:@selector(didClose:) name:NSWindowWillCloseNotification object:self];
        
        self->m_handle = handle;
        [self setReleasedWhenClosed:NO];
    }
    
    return self;
}

- (void)didMiniaturize:(NSNotification *)notification
{
    nui::Window::Window *window = (nui::Window::Window *)self->m_handle;
    
    if(window) {
        window->Emit("miniaturize");
    }
}

- (void)didDeminiaturize:(NSNotification *)notification
{
    nui::Window::Window *window = (nui::Window::Window *)self->m_handle;
    
    if(window) {
        window->Emit("deminiaturize");
    }
}

- (void)didClose:(NSNotification *)notification
{
    nui::Window::Window *window = (nui::Window::Window *)self->m_handle;
    
    if(window) {
        window->Emit("close", nui::MacWindow::CloseCallback, window);
    }
}

#pragma mark NSObject

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    
    [super dealloc];
}

@end
