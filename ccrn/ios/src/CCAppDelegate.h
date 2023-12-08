/****************************************************************************
 Copyright (c) 2010-2013 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You
shall not use Cocos Creator software for developing other software or tools
that's used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to
you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#pragma once

#import <UIKit/UIKit.h>
@class AppDelegateBridge;
@class ViewController;
@interface CCAppDelegate : NSObject <UIApplicationDelegate> {}

@property(nonatomic) NSString *_rootSearch;
@property(nonatomic, readonly) ViewController *viewController;
@property(nonatomic, readonly) AppDelegateBridge *appDelegateBridge;
@property (nullable, nonatomic, strong) UIWindow *reactNativeWindow API_AVAILABLE(ios(5.0));
@property (nullable, nonatomic, strong) UIWindow *cocosWindow API_AVAILABLE(ios(5.0));
@property (nonatomic, strong) UIImageView *logoImageView;
@property (nonatomic, strong) UILabel *loadingLabel;
@property (nonatomic, strong) UIProgressView *progressBar;

+ (nonnull instancetype)getInstance;
- (void)initPlatform;
- (void)startGame:(NSString*) gameId gameData:(NSString*) gameData;
- (void)stopGame;
- (void)onGameLoaded;
- (void)setLoadingPercent:(float)percent;
- (void)setLoadingVisible:(BOOL)visible;
- (void)setProgressBarAndTextVisible:(BOOL)visible;
- (void)onStartGameFailed;
@end
