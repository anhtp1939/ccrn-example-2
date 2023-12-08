#pragma once
class CCLoadingWrapper
{
public:
    static void setLoadingPercent(float percent);
    static void hideLoadingLayer();
    static void showLoadingLayer();
    static void setLoadingVisible(bool visible);
    static void setProgressBarAndTextVisible(bool visible);
    static void onStartGameFailed();
};