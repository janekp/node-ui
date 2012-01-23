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

#include <pthread.h>
#include "nui.h"
#include "nui_application.h"
#include "nui_asset.h"
#include "nui_buffer.h"
#include "nui_menu.h"
#include "nui_view.h"
#include "nui_window.h"

namespace nui {
    int Ready = 0;
    
    typedef struct _Context {
        int argc;
        char **argv;
    } Context;
    
    static void Initialize(v8::Handle<v8::Object> target) {
        Application::Initialize(target);
        Menu::Initialize(target);
        View::Initialize(target);
        Window::Initialize(target);
        WebWindow::Initialize(target);
    }
    
    static void InitializeExtensions(v8::Handle<v8::Object> target) {
        Asset::Initialize(target);
        Buffer::Initialize(target);
    }
    
    static void Complete(uv_timer_t *handle, int status) {
        nui::Ready = 1;
    }
    
    static void *ExecuteOnThread(void *ptr) {
        Context *ctx = (Context *)ptr;
        uv_timer_t timer;
        
        // Custom emitter support
        EventEmitter::Register();
        
        // The caller thread is blocked after node::Start
        uv_timer_init(uv_default_loop(), &timer);
        uv_unref(uv_default_loop());
        uv_timer_start(&timer, Complete, 1, 0);
        
        // Register 'ui' module
        node::register_dynamic_module(&ui_module);
        
        // Register 'uix' module
        node::register_dynamic_module(&uix_module);
        
        // Run everything
        node::Start(ctx->argc, ctx->argv);
        
        return NULL;
    }
    
    int Execute(int argc, char *argv[]) {
        Context *ctx = (Context *)malloc(sizeof(Context));
        pthread_t thread;
        
        // Spawn a new thread
        ctx->argc = argc;
        ctx->argv = (char **)argv;
        
        pthread_create(&thread, NULL, &ExecuteOnThread, ctx);
        
        return 0;
    }
}

NODE_MODULE(ui, nui::Initialize);
NODE_MODULE(uix, nui::InitializeExtensions);
