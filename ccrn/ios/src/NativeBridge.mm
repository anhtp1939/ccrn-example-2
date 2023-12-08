#import "NativeBridge.h"
#include "cocos/application/ApplicationManager.h"
#import "cocos/platform/apple/JsbBridgeWrapper.h"
#import "CCAppDelegate.h"

@implementation NativeBridge
{
    BOOL _isInGame;
    __unsafe_unretained id<NativeBridgeDelegate> _delegate;
}

static NativeBridge *instance = nil;

+ (instancetype)sharedInstance
{
    static dispatch_once_t pred = 0;
    dispatch_once(&pred, ^{
        instance = [[super allocWithZone:NULL] init];
        NSAssert(instance != nil, @"alloc or init failed");
    });
    return instance;
}

-(instancetype)init
{
    self = [super init];
    if (self) {
        _isInGame = NO;
    }
    return self;
}

-(void)onScriptEvent:(NSString*)eventName eventData:(NSString*)eventData
{
    NSLog(@"[NativeBridge] onScriptEvent: %@, %@", eventName, eventData);
    if ([eventName isEqualToString:@"EVENT_STOP_GAME"])
    {
        [[CCAppDelegate getInstance] stopGame];
        return;
    }
    if ([eventName isEqualToString:@"EVENT_GAME_LOADED"])
    {
        [[CCAppDelegate getInstance] onGameLoaded];
        return;
    }
    
    if (_delegate && [_delegate respondsToSelector:@selector(onScriptEvent:eventData:)])
    {
        [_delegate onScriptEvent:eventName eventData:eventData];
    }
}

-(void)startGame
{
    NSLog(@"[NativeBridge] startGame; %d", _isInGame);
    NSLog(@"[NativeBridge] Set JsbBridge callback");
    [[JsbBridge sharedInstance] setCallback:^void (NSString *arg0, NSString *arg1) {
        [[NativeBridge sharedInstance] onScriptEvent:arg0 eventData:arg1];
    }];
    _isInGame = YES;
}

-(void)stopGame
{
    NSLog(@"[NativeBridge] stopGame %d", _isInGame);
    [[JsbBridge sharedInstance] setCallback:nil];
    _isInGame = NO;
}

-(void)dispatchEventToScript:(NSString*)eventName eventData:(NSString*)eventData
{
    NSLog(@"[NativeBridge] dispatchEventToScript: %@, %@", eventName, eventData);
    if (_isInGame) {
        [[JsbBridge sharedInstance] sendToScript:eventName arg1:eventData];
    }
}

- (void)setDelegate:(id<NativeBridgeDelegate>)delegate
{
    _delegate = delegate;
}

- (void)onStartGameFailed:(NSString *)reason
{
    if (_delegate && [_delegate respondsToSelector:@selector(onStartGameFailed:)])
    {
        [_delegate onStartGameFailed:reason];
    }
}

@end
