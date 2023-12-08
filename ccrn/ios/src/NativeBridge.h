#import <Foundation/Foundation.h>
#include "cocos/platform/apple/JsbBridge.h"

@protocol NativeBridgeDelegate <NSObject>
- (void)onScriptEvent:(NSString *)eventName eventData:(NSString *)eventData;
- (void)onStartGameFailed:(NSString *)reason;
@end

@interface NativeBridge : NSObject
+(instancetype)sharedInstance;
-(void)onScriptEvent:(NSString*)eventName eventData:(NSString*)eventData;
-(void)startGame;
-(void)stopGame;
-(void)onGameLoaded;
-(void)dispatchEventToScript:(NSString*)eventName eventData:(NSString*)eventData;
-(void)setDelegate:(id<NativeBridgeDelegate>)delegate;
-(void)onStartGameFailed:(NSString *)reason;
@end
