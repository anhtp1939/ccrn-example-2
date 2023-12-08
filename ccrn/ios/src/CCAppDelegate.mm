/****************************************************************************
 Copyright (c) 2010-2013 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#import "CCAppDelegate.h"

#include "platform/ios/AppDelegateBridge.h"
#include "platform/ios/View.h"
#include "platform/ios/IOSPlatform.h"
#include "platform/ios/modules/SystemWindow.h"
#include "cocos/application/ApplicationManager.h"

#include "ViewController.h"
#include "Game.h"
#include "SDKWrapper.h"
#import "NativeBridge.h"


@implementation CCAppDelegate
@synthesize window;
@synthesize appDelegateBridge;

#pragma mark -
#pragma mark Application lifecycle

static CCAppDelegate *mInstance = nil;

+ (nonnull instancetype)getInstance
{   
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
      mInstance = [[super allocWithZone:NULL] init];
    });
    return mInstance;
}

- (void)setupCocosGameWindow {
    // Create window
    CGRect bounds = [[UIScreen mainScreen] bounds];
    self.cocosWindow = [[UIWindow alloc] initWithFrame:bounds];
    
    // Create view controller
    _viewController = [[ViewController alloc] init];
    _viewController.view = [[View alloc] initWithFrame:bounds];
    _viewController.view.contentScaleFactor = UIScreen.mainScreen.scale;
    _viewController.view.multipleTouchEnabled = true;
    _viewController.view.backgroundColor = [UIColor colorWithRed:26/255.0 green:26/255.0 blue:26/255.0 alpha:1.0];

    // Add the logo, label, and progress bar to the view controller's view
    self.logoImageView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"logo.png"]];
    self.logoImageView.contentMode = UIViewContentModeScaleAspectFit;
    self.logoImageView.translatesAutoresizingMaskIntoConstraints = NO;
    self.logoImageView.transform = CGAffineTransformMakeScale(0.5, 0.5);
    [_viewController.view addSubview:self.logoImageView];
    
    self.loadingLabel = [[UILabel alloc] init];
    self.loadingLabel.text = @"Loading...";
    self.loadingLabel.textAlignment = NSTextAlignmentCenter;
    self.loadingLabel.translatesAutoresizingMaskIntoConstraints = NO;
    self.loadingLabel.textColor = [UIColor whiteColor];
    [_viewController.view addSubview:self.loadingLabel];
    
    self.progressBar = [[UIProgressView alloc] initWithProgressViewStyle:UIProgressViewStyleDefault];
    self.progressBar.translatesAutoresizingMaskIntoConstraints = NO;
    [_viewController.view addSubview:self.progressBar];
    
    // Center the logo image view
    [NSLayoutConstraint activateConstraints:@[
        [self.logoImageView.centerXAnchor constraintEqualToAnchor:_viewController.view.centerXAnchor],
        [self.logoImageView.centerYAnchor constraintEqualToAnchor:_viewController.view.centerYAnchor constant:-50.0]
    ]];
    
    // Center the loading label
    [NSLayoutConstraint activateConstraints:@[
        [self.loadingLabel.centerXAnchor constraintEqualToAnchor:_viewController.view.centerXAnchor],
        [self.loadingLabel.centerYAnchor constraintEqualToAnchor:_viewController.view.centerYAnchor constant:125.0]
    ]];
    
    // Center the progress bar
    [NSLayoutConstraint activateConstraints:@[
        [self.progressBar.centerXAnchor constraintEqualToAnchor:_viewController.view.centerXAnchor],
        [self.progressBar.centerYAnchor constraintEqualToAnchor:_viewController.view.centerYAnchor constant:100.0],
        [self.progressBar.widthAnchor constraintEqualToAnchor:_viewController.view.widthAnchor multiplier:0.8]
    ]];
    
    // Set the root view controller and make the window visible
    [self.cocosWindow setRootViewController:_viewController];
    [self.cocosWindow makeKeyAndVisible];

    [self setProgressBarAndTextVisible:NO];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self initPlatform];
    
    // Init SDK
    [[SDKWrapper shared] application:application didFinishLaunchingWithOptions:launchOptions];
    appDelegateBridge = [[AppDelegateBridge alloc] init];

    // Save react native window pointer
    self.reactNativeWindow = UIApplication.sharedApplication.delegate.window;

    // Call AppDelegateBridge to init and run the engine
    [appDelegateBridge application:application didFinishLaunchingWithOptions:launchOptions];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    [[SDKWrapper shared] applicationWillResignActive:application];
    [appDelegateBridge applicationWillResignActive:application];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    [[SDKWrapper shared] applicationDidBecomeActive:application];
    [appDelegateBridge applicationDidBecomeActive:application];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    [[SDKWrapper shared] applicationDidEnterBackground:application];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    [[SDKWrapper shared] applicationWillEnterForeground:application];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [[SDKWrapper shared] applicationWillTerminate:application];
    [appDelegateBridge applicationWillTerminate:application];
}

#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    [[SDKWrapper shared] applicationDidReceiveMemoryWarning:application];
}

- (void)initPlatform
{
    cc::BasePlatform *platform = cc::BasePlatform::getPlatform();
    if (platform->init())
    {
        NSLog(@"Platform init went wrong -1");
        return;
    }
    platform->run(0, nullptr);
}

- (void)startGame:(NSString *)gameId gameData:(NSString *)gameData
{
    NSLog(@"[CCAppDelegate] <startGame> %@ %@", gameId, gameData);
    dispatch_async(dispatch_get_main_queue(), ^{
        
        // Save the previous windows of React Native side to re-activate when exit game
        self.reactNativeWindow = UIApplication.sharedApplication.delegate.window;
        // Make cocos window 'key' window and show it
        [self setupCocosGameWindow];
        // Mark the current active window of main AppDelegate is Cocos Window (this will be used in cc::SystemWindow)
        UIApplication.sharedApplication.delegate.window = self.cocosWindow;

        auto app = std::static_pointer_cast<Game>(CC_APPLICATION_MANAGER()->getCurrentApp());
        if (app)
        {
            app->startGame(gameId.UTF8String, gameData.UTF8String);
            [[NativeBridge sharedInstance] startGame];
        }
    });
}

- (void)stopGame
{
    NSLog(@"[CCAppDelegate] <stopGame>");
    dispatch_async(dispatch_get_main_queue(), ^{
        [_viewController.view removeFromSuperview];
        // Make react-native window 'key' and show it
        [self.reactNativeWindow makeKeyAndVisible];
        // Mark the current active window of main AppDelegate is React Native Window (this will be used in cc::SystemWindow)
        UIApplication.sharedApplication.delegate.window = self.reactNativeWindow;
    });
    auto app = std::static_pointer_cast<Game>(CC_APPLICATION_MANAGER()->getCurrentApp());
    if (app)
    {
        app->restart();
        [[NativeBridge sharedInstance] stopGame];
    }
}

- (void)onGameLoaded
{
    NSLog(@"[CCAppDelegate] <onGameLoaded>");
    auto app = std::static_pointer_cast<Game>(CC_APPLICATION_MANAGER()->getCurrentApp());
    if (app)
    {
        app->onGameLoaded();
    }
}

- (void)setLoadingPercent:(float)percent
{
    if (self.progressBar != nil)
    {
        [self.progressBar setProgress:percent animated:YES];
    }
}

- (void)setLoadingVisible:(BOOL)visible
{
    if (self.progressBar != nil)
    {
        self.progressBar.hidden = !visible;
    }
    if (self.logoImageView != nil)
    {
        self.logoImageView.hidden = !visible;
    }
    if (self.loadingLabel != nil)
    {
        self.loadingLabel.hidden = !visible;
    }
}

- (void)setProgressBarAndTextVisible:(BOOL)visible
{
    if (self.progressBar != nil)
    {
        self.progressBar.hidden = !visible;
    }
    if (self.loadingLabel != nil)
    {
        self.loadingLabel.hidden = !visible;
    }
}

- (void)onStartGameFailed
{
    [[NativeBridge sharedInstance] onStartGameFailed:@"Game Download Failed!"];
    [self stopGame];
}

@end
