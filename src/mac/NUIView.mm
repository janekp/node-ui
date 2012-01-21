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
    static void ExecScriptCallback(EventEmitter *emitter, void *context, const v8::Persistent<v8::Function> &function) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        Function *ctx = (Function *)context;
        
        if(emitter) {
            v8::Local<v8::String> str = v8::String::New([(NSString *)ctx->info UTF8String]);
            v8::Handle<v8::Value> args[1] = { str };
            
            ctx->data->Call(emitter->handle_, 1, args);
        } else {
            [(id)ctx->info release];
            
            ctx->data.Dispose();
            ctx->data.Clear();
            free(ctx);
        }
        
        [pool release];
    }
    
    class MacView : public View {
    public:
        MacView(const v8::Local<v8::Object> &handle);
        virtual ~MacView();
        virtual void *GetImpl();
        virtual void Exec(const char *str, v8::Handle<v8::Value> func);
        virtual void Load(const char *path);
        virtual void Load(const void *data, int length);
        
    protected:
        NUIView *m_impl;
        int m_ref;
    };
    
    MacView::MacView(const v8::Local<v8::Object> &handle): View(handle) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        this->m_impl = (NUIView *)[NUIProxy allocWithName:@"NUIView"];
        this->m_ref = 0;
        [this->m_impl initWithHandle:this];
        
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
    
    void MacView::Exec(const char *str, v8::Handle<v8::Value> func) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSString *script = [[NSString alloc] initWithCString:str encoding:NSUTF8StringEncoding];
        Function *_func = NULL;
        
        if(func->IsFunction()) {
            _func = (Function *)malloc(sizeof(Function));
            _func->data = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(func));
            _func->info = NULL;
        }
        
        [this->m_impl execScript:script callback:_func];
        [script release];
        [pool release];
    }
    
    void MacView::Load(const char *str) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSString *path = [[NSString alloc] initWithCString:str encoding:NSUTF8StringEncoding];
        
        [this->m_impl loadFileAtPath:path];
        [path release];
        [pool release];
    }
    
    void MacView::Load(const void *data, int length) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        NSString *str = [[NSString alloc] initWithBytesNoCopy:(void *)data length:length encoding:NSUTF8StringEncoding freeWhenDone:NO];
        
        [this->m_impl loadHTMLString:str];
        [str release];
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
        [self setFrameLoadDelegate:self];
    }
    
    return self;
}

- (void)execScript:(NSString *)script callback:(void *)callback
{
    NSString *result = [self stringByEvaluatingJavaScriptFromString:script];
    
    if(callback) {
        nui::Function *func = (nui::Function *)callback;
        nui::MacView *view = (nui::MacView *)self->m_handle;
        
        if(view != NULL) {
            func->info = (void *)[result retain];
            view->Emit(nui::ExecScriptCallback, func);
        } else {
            func->data.Dispose();
            func->data.Clear();
            free(func);
        }
    }
}

- (void)loadFileAtPath:(NSString *)path
{
    [[self mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:path]]];
}

- (void)loadHTMLString:(NSString *)str
{
    [[self mainFrame] loadHTMLString:str baseURL:nil];
}

#pragma mark WebFrameLoadDelegate

- (void)webView:(WebView *)sender willPerformClientRedirectToURL:(NSURL *)URL delay:(NSTimeInterval)seconds fireDate:(NSDate *)date forFrame:(WebFrame *)frame
{
    if([URL.scheme isEqualToString:@"nui"]) {
        nui::MacView *view = (nui::MacView *)self->m_handle;
        
        if(view) {
            view->Emit("data", [[[URL absoluteString] substringFromIndex:6] retain]);
        }
    }
}

- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
    nui::MacView *view = (nui::MacView *)self->m_handle;
    
    if(view) {
        view->Emit("ready");
    }
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
    nui::MacView *view = (nui::MacView *)self->m_handle;
    
    if(view) {
        view->Emit("error");
    }
}

@end
