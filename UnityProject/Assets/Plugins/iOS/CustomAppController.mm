#import <UIKit/UIKit.h>
#import "UnityAppController.h"

extern "C" void UnitySetGraphicsDevice_iOS(void* device, int deviceType, int eventType);
extern "C" void UnityRenderMarker_iOS(int marker);

@interface MyAppController : UnityAppController
{
}
- (void)shouldAttachRenderDelegate;
@end

@implementation MyAppController

- (void)shouldAttachRenderDelegate;
{
    UnityRegisterRenderingPlugin(UnitySetGraphicsDevice_iOS, UnityRenderMarker_iOS);
}
@end


IMPL_APP_CONTROLLER_SUBCLASS(MyAppController)
