#import "CCLoadingWrapper.h"
#import "CCAppDelegate.h"

void CCLoadingWrapper::setLoadingPercent(float percent)
{
    [[CCAppDelegate getInstance] setLoadingPercent:percent];
}

void CCLoadingWrapper::hideLoadingLayer()
{
    [[CCAppDelegate getInstance] setLoadingVisible:NO];
}

void CCLoadingWrapper::showLoadingLayer()
{
    [[CCAppDelegate getInstance] setLoadingVisible:YES];
}

void CCLoadingWrapper::setLoadingVisible(bool visible)
{
    [[CCAppDelegate getInstance] setLoadingVisible:visible];
}

void CCLoadingWrapper::setProgressBarAndTextVisible(bool visible)
{
    [[CCAppDelegate getInstance] setProgressBarAndTextVisible:visible];
}

void CCLoadingWrapper::onStartGameFailed()
{
    [[CCAppDelegate getInstance] onStartGameFailed];
}
