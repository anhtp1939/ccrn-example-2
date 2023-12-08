#ifndef CCRNModule_h
#define CCRNModule_h

#import <React/RCTBridgeModule.h>
#import <React/RCTEventEmitter.h>
#import "NativeBridge.h"

@interface CCRNModule : RCTEventEmitter <RCTBridgeModule, NativeBridgeDelegate>
- (void)onScriptEvent:(NSString *)eventName eventData:(NSString *)eventData;
- (void)sendEventToReact:(NSString *)eventName eventData:(NSString *)eventData;
- (void)onStartGameFailed:(NSString *) reason;
@end

#endif

