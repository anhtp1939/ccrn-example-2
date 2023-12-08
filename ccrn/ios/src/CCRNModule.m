#import "CCRNModule.h"
#import <UIKit/UIKit.h>
#import <React/RCTLog.h>
#import "CCAppDelegate.h"
#import "NativeBridge.h"

@implementation CCRNModule

-(instancetype)init {
    self = [super init];
    if (self) {
        [[NativeBridge sharedInstance] setDelegate:self];
    }
    return self;
}

RCT_EXPORT_MODULE(CCRNModule);

RCT_EXPORT_METHOD(startGame:(NSString *)gameId data:(NSString*)data)
{
    [[CCAppDelegate getInstance] startGame:gameId gameData:data];
}

RCT_EXPORT_METHOD(dispatchEventToCocos:(NSString *)eventName eventData:(NSString *)eventData)
{
    NSLog(@"[CCRNModule] <dispatchEventToCocos> %@, %@", eventName, eventData);
    [[NativeBridge sharedInstance] dispatchEventToScript:eventName eventData:eventData];
}

- (void)onScriptEvent:(NSString *)eventName eventData:(NSString *)eventData
{
    [self sendEventWithName:eventName body:eventData];
}

- (void)sendEventToReact:(NSString *)eventName eventData:(NSString *)eventData
{
    [self sendEventWithName:eventName body:eventData];
}

- (void)onStartGameFailed:(NSString *) reason
{
    NSLog(@"[CCRNModule] <onStartGameFailed> %@", reason);
    [self sendEventToReact:@"ON_START_GAME_FAILED" eventData:reason];
}

- (NSArray<NSString *> *)supportedEvents
{
    return @[
        @"EVENT_FROM_COCOS_TO_REACT",
        @"EVENT_FROM_REACT_TO_COCOS",
        @"EVENT_STOP_GAME",
        @"ON_START_GAME_FAILED"
    ];
}

@end
