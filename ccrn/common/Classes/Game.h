/****************************************************************************
 Copyright (c) 2018 Xiamen Yaji Software Co., Ltd.

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

#include <network/HttpClient.h>
#include "cocos/cocos.h"

/**
 @brief    The cocos2d Application.

 The reason for implement as private inheritance is to hide some interface call
 by Director.
 */
class Game : public cc::BaseGame
{
public:
    Game();
    int init() override;
    // bool init() override;
    void onPause() override;
    void onResume() override;
    void onClose() override;

    void startGame(const std::string &gameId, const std::string &gameData);
    bool hasGame(const std::string &gameId);
    void runGame(const std::string &gameId);
    void onGameLoaded();
    void checkForUpdate(const std::string &gameId);
    void downloadGame(const std::string &gameId, const std::string &url);
    void unzip();
    void updateVersion();
    void parseHostFromGameData();
    void onCheckForUpdateResponse(cc::network::HttpClient *sender, cc::network::HttpResponse *response);
    void setLoadingPercent(float percent);
    void setLoadingVisible(bool visible);
    void setProgressBarAndTextVisible(bool visible);
    void onStartGameFailed();
    std::string getVersionKey(const std::string &gameId);
    std::string getLocalVersion(const std::string &gameId);
    
    static std::string decodeURL(const std::string& url);

private:
    static std::string GAME_ID;
    static std::string GAME_DATA;
    static std::string HOST;

    std::string _localVersion;
    std::string _remoteVersion;
};
