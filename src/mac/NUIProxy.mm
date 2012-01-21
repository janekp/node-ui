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

@implementation NUIProxy

+ (id)allocWithClass:(Class)klass
{
    return [[self alloc] initWithClass:klass];
}

+ (id)allocWithName:(NSString *)name
{
    return [[self alloc] initWithName:name];
}

- (id)initWithName:(NSString *)name
{
    self = [super init];
    
    if(self) {
        [self performSelectorOnMainThread:@selector(pm_object:) withObject:name waitUntilDone:YES];
    }
    
    return self;
}

- (id)initWithClass:(Class)klass
{
    self = [super init];
    
    if(self) {
        self->m_object = [klass alloc];
    }
    
    return self;
}

- (void)pm_object:(NSString *)name
{
    self->m_object = [NSClassFromString(name) alloc];
}

- (void)pm_call:(NSInvocation *)invocation
{
    [invocation invokeWithTarget:self->m_object];
}

#pragma NSObject

- (void)forwardInvocation:(NSInvocation *)invocation 
{
	SEL selector = [invocation selector];
	
	if([self->m_object respondsToSelector:selector]) {
        [self performSelectorOnMainThread:@selector(pm_call:) withObject:invocation waitUntilDone:YES];
	} else {
		[self doesNotRecognizeSelector:selector];
	}
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)selector
{
	if([self->m_object respondsToSelector:selector]) {
		return [self->m_object methodSignatureForSelector:selector];
	}
	
	return nil;
}

- (void)dealloc
{
    [self->m_object performSelectorOnMainThread:@selector(release) withObject:nil waitUntilDone:NO];
    
    [super dealloc];
}

@end
