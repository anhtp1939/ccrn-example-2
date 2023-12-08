/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "Game.h"
#if (CC_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/java/jni/JniHelper.h"
#include <audio/android/cutils/log.h>
#endif
#if (CC_PLATFORM == CC_PLATFORM_IOS)
#include "../../ios/src/CCLoadingWrapper.h"
#endif
#include <application/ApplicationManager.h>
#include <FileUtils.h>
#include <base/UTF8.h>
#include <rapidjson/document.h>
#include <network/Downloader.h>
#include <base/ZipUtils.h>
#include <storage/local-storage/LocalStorage.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstring>

#ifndef GAME_NAME
#define GAME_NAME "CocosGame";
#endif

#ifndef SCRIPT_XXTEAKEY
#define SCRIPT_XXTEAKEY "25eRqcjpRCK4rnvA";
#endif

#if (CC_PLATFORM == CC_PLATFORM_IOS)
#define PLATFORM_NAME "ios"
#else
#define PLATFORM_NAME "android"
#endif

#define CCRN_CLASSNAME "com/ccrn/CCRNActivity"

std::string Game::GAME_ID = "game_id";
std::string Game::GAME_DATA = "";
std::string Game::HOST = "http://35.238.218.10:3001"; // default host

Game::Game() = default;

bool isURLEncoded(const char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == '%') {
            // Check if the next two characters are valid hexadecimal digits
            if (!isxdigit(str[i+1]) || !isxdigit(str[i+2])) {
                return false;
            }
            i += 2; // Skip the next two characters
        }
    }
    return true;
}

int Game::init()
{
    _windowInfo.title = GAME_NAME;

#if CC_DEBUG
    _debuggerInfo.enabled = true;
#else
    _debuggerInfo.enabled = false;
#endif
    _debuggerInfo.port = 6086;
    _debuggerInfo.address = "0.0.0.0";
    _debuggerInfo.pauseOnStart = false;

    _xxteaKey = SCRIPT_XXTEAKEY;

    CC_LOG_INFO("[Game] init");
    BaseGame::init();
    return 0;
}

void Game::onPause() { BaseGame::onPause(); }

void Game::onResume() { BaseGame::onResume(); }

void Game::onClose() { BaseGame::onClose(); }

void Game::startGame(const std::string &gameId, const std::string &gameData)
{
    CC_LOG_INFO("Game::startGame(%s, %s)", gameId.c_str(), gameData.c_str());
    GAME_ID = gameId;
    GAME_DATA = gameData;
    this->parseHostFromGameData();
    _localVersion = getLocalVersion(GAME_ID);
    checkForUpdate(GAME_ID);
}

bool Game::hasGame(const std::string &gameId)
{
    const auto fileUtils = cc::FileUtils::getInstance();
    const auto gamePath = "games/" + gameId;
    const auto cachePath = fileUtils->getWritablePath() + "downloads/games/" + gameId;
    CC_LOG_INFO("Game::hasGame(%s) - %d", gameId.c_str(), fileUtils->isDirectoryExist(gameId));
    CC_LOG_INFO("Game::hasGame(%s) - %d", cachePath.c_str(), fileUtils->isDirectoryExist(cachePath));
    return fileUtils->isDirectoryExist(gameId) || fileUtils->isDirectoryExist(cachePath);
}

void Game::runGame(const std::string &gameId)
{
    if (!hasGame(gameId))
    {
        CC_LOG_WARNING("Game::runGame(%s) Game not found!!", gameId.c_str());
        this->onStartGameFailed();
        return;
    }
    CC_LOG_INFO("Game::runGame(%s)", gameId.c_str());

    // Update Search Paths for current game
    const auto fileUtils = cc::FileUtils::getInstance();
    fileUtils->setSearchPaths(fileUtils->getOriginalSearchPaths());
    CC_LOG_INFO("Game::setSearchPaths(fileUtils->getOriginalSearchPaths())");
    const auto gamePath = "games/" + gameId;
    const auto cachePath = fileUtils->getWritablePath() + "downloads/games/" + gameId;
    fileUtils->addSearchPath(gamePath);
    fileUtils->addSearchPath(cachePath);
    CC_LOG_INFO("Game::addSearchPath(%s)", gamePath.c_str());
    CC_LOG_INFO("Game::addSearchPath(%s)", cachePath.c_str());

    // Run JS Entry
    runScript("jsb-adapter/web-adapter.js");
    runScript("main.js");
    CC_LOG_INFO("Game::startGames runJS done");
}

void Game::onGameLoaded()
{
    this->setLoadingVisible(false);
    CC_CURRENT_ENGINE()->getScheduler()->performFunctionInCocosThread([=]()
                                                                      {
        const auto& jsString = cc::StringUtils::format("window.onGameDataFromNative('%s')", GAME_DATA.c_str());
        se::ScriptEngine::getInstance()->evalString(jsString.c_str()); });
}

void Game::checkForUpdate(const std::string &gameId)
{
    CC_LOG_DEBUG("Game::checkForUpdate(%s) localVersion = %s", gameId.c_str(), _localVersion.c_str());
    const auto &url = cc::StringUtils::format("%s/projects/%s/configs", HOST.c_str(), gameId.c_str());
    CC_LOG_DEBUG("Game::checkForUpdate url = %s", url.c_str());
    cc::network::HttpRequest *request = new (std ::nothrow) cc::network::HttpRequest();
    request->setUrl(url);
    request->setRequestType(cc::network::HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(Game::onCheckForUpdateResponse, this));
    cc::network::HttpClient::getInstance()->send(request);
}

void Game::onCheckForUpdateResponse(cc::network::HttpClient *sender, cc::network::HttpResponse *response)
{
    if (!response || !response->isSucceed())
    {
        const auto errorCode = response ? response->getResponseCode() : -1939L;
        CC_LOG_WARNING("Game::onCheckForUpdateResponse(%s) Failed!, error code is %ld", GAME_ID.c_str(), errorCode);
        // Check for update failed, try to run the game anyway
        runGame(GAME_ID);
        return;
    }
    const auto responseData = response->getResponseData();
    responseData->push_back(0);
    const auto data = response->getResponseData()->data();
    rapidjson::Document document;
    CC_LOG_WARNING("Game::onCheckForUpdateResponse() jsonString %s", data);
    document.Parse(data);
    if (document.HasParseError())
    {
        CC_LOG_WARNING("Game::onCheckForUpdateResponse(%s) ParseJsonFailed", GAME_ID.c_str());
        runGame(GAME_ID);
        return;
    }
    const std::string currentPlatform = PLATFORM_NAME;
    if (document.HasMember("bundles") && document["bundles"].IsArray())
    {
        const rapidjson::Value &bundles = document["bundles"];
        for (rapidjson::SizeType i = 0; i < bundles.Size(); i++)
        {
            const rapidjson::Value &bundle = bundles[i];
            if (bundle.HasMember("fileType") && bundle["fileType"].IsString() &&
                bundle.HasMember("version") && bundle["version"].IsString() &&
                bundle.HasMember("url") && bundle["url"].IsString())
            {
                std::string fileType = bundle["fileType"].GetString();
                _remoteVersion = bundle["version"].GetString();
                std::string url = bundle["url"].GetString();

                if (fileType == currentPlatform)
                {
                    if (_remoteVersion != _localVersion)
                    {
                        downloadGame(GAME_ID, url);
                        return;
                    }
                    runGame(GAME_ID);
                    return;
                }
            }
        }
    }
    // Can not find any bundle with currentPlatform, Just run the game
    runGame(GAME_ID);
}

std::string Game::decodeURL(const std::string &url)
{
    std::ostringstream decodedUrl;
    std::istringstream inputStream(url);
    char currentChar;
    int hexValue;
    while (inputStream >> std::noskipws >> currentChar) {
        if (currentChar == '%') {
            if (!(inputStream >> std::hex >> hexValue)) {
                decodedUrl << currentChar;
                continue;
            }
            decodedUrl << static_cast<char>(hexValue);
            inputStream >> std::resetiosflags(std::ios_base::basefield);
        } else if (currentChar == '+') {
            decodedUrl << ' ';
        } else {
            decodedUrl << currentChar;
        }
    }

    return decodedUrl.str();
}

void Game::downloadGame(const std::string &gameId, const std::string &url)
{
    this->setProgressBarAndTextVisible(true);
    CC_LOG_INFO("Game::downloadGame - url = %s", url.c_str());
    const auto fileName = gameId + ".zip";
#if (CC_PLATFORM == CC_PLATFORM_IOS)
    // const auto isEncoded = isURLEncoded(url.c_str());
    // const auto targetURL = isEncoded ? decodeURL(url) : url;
    const auto targetURL = url;
    CC_LOG_INFO("Game::downloadGame - targetURL = %s", targetURL.c_str());
#else
    const auto targetURL = url;
#endif

    const auto fileUtils = cc::FileUtils::getInstance();
    const auto gameFolder = fileUtils->getWritablePath() + "downloads/games/";
    if (!fileUtils->isDirectoryExist(gameFolder))
    {
        fileUtils->createDirectory(gameFolder);
    }
    const auto filePath = gameFolder + fileName;
    CC_LOG_INFO("Game::downloadGame() filePath = %s", filePath.c_str());
    cc::network::Downloader *downloader = new (std::nothrow) cc::network::Downloader();
    downloader->onTaskProgress = ([this](const cc::network::DownloadTask &task, int64_t bytesReceived, int64_t totalBytesReceived, int64_t totalBytesExpected)
    {
        CC_LOG_DEBUG("Downloaded %d/%d bytes", totalBytesReceived, totalBytesExpected);
        const auto percent = totalBytesExpected > 0 ? totalBytesReceived * 1.0f / totalBytesExpected : 1.0f;
        this->setLoadingPercent(percent);
    });

    downloader->onFileTaskSuccess = ([this](const cc::network::DownloadTask &task)
                                     {
        CC_LOG_DEBUG("File Downloaded!!!!");
        unzip();
        runGame(GAME_ID);
        updateVersion(); });

    downloader->onTaskError = ([this](const cc::network::DownloadTask &task, int errorCode, int errorCodeInternal, const std::string &errorStr)
                               {
        CC_LOG_WARNING("Download Error %d %d %s", errorCode, errorCodeInternal, errorStr.c_str());
        runGame(GAME_ID); });
    downloader->createDownloadTask(targetURL,filePath);
}

void Game::unzip()
{
    const auto zipFileName = GAME_ID + ".zip";
    const auto fileUtils = cc::FileUtils::getInstance();
    const auto folderPath = fileUtils->getWritablePath() + "downloads/games/";
    const auto gameFolderPath = folderPath + GAME_ID;
    const auto filePath = folderPath + zipFileName;
    // Delete old files before unzip new version
    if (fileUtils->isDirectoryExist(gameFolderPath))
    {
        CC_LOG_DEBUG("Remove old folder %s", gameFolderPath.c_str());
        fileUtils->removeDirectory(gameFolderPath);
    }
    cc::ZipFile zFile = cc::ZipFile(filePath);
    std::string fileName = zFile.getFirstFilename();
    std::string file = fileName;
    unsigned int fileSize;
    unsigned char *data = zFile.getFileData(fileName, &fileSize);
    while (data != nullptr)
    {
        std::string fullFileName = folderPath + file;
        if (fullFileName[fullFileName.size() - 1] == '/')
        {
            CC_LOG_DEBUG("create dir: %s", fullFileName.c_str());
            fileUtils->createDirectory(fullFileName);
            free(data);
            fileName = zFile.getNextFilename();
            file = fileName;
            data = zFile.getFileData(fileName, &fileSize);
            continue;
        }
        CC_LOG_DEBUG("write file: %s", fullFileName.c_str());
        FILE *fp = fopen(fullFileName.c_str(), "wb");
        if (fp)
        {
            fwrite(data, 1, fileSize, fp);
            fclose(fp);
        }
        free(data);
        fileName = zFile.getNextFilename();
        file = fileName;
        data = zFile.getFileData(fileName, &fileSize);
    }
    fileUtils->removeFile(filePath);
}

void Game::updateVersion()
{
    _localVersion = _remoteVersion;
    const auto versionKey = getVersionKey(GAME_ID);
    const auto versionString = cc::StringUtils::toString(_localVersion);
    CC_LOG_DEBUG("Update %s version to %s", GAME_ID.c_str(), versionString.c_str());
    localStorageSetItem(versionKey, versionString);
}

std::string Game::getVersionKey(const std::string &gameId)
{
    return gameId + "_version";
}

std::string Game::getLocalVersion(const std::string &gameId)
{
    std::string versionStr;
    const auto versionKey = getVersionKey(gameId);
    const auto hasVersion = localStorageGetItem(versionKey, &versionStr);
    return hasVersion ? versionStr : "0.0.0";
}

void Game::parseHostFromGameData()
{
    rapidjson::Document document;
    document.Parse(GAME_DATA.c_str());
    if (document.HasMember("host") && document["host"].IsString())
    {
        HOST = document["host"].GetString();
    }
    CC_LOG_INFO("[Game] <parseHostFromGameData> HOST = %s", HOST.c_str());
}

void Game::setLoadingPercent(float percent)
{
#if (CC_PLATFORM == CC_PLATFORM_IOS)
    CCLoadingWrapper::setLoadingPercent(percent);
#endif
#if (CC_PLATFORM == CC_PLATFORM_ANDROID)
    cc::JniHelper::callObjectVoidMethod(cc::JniHelper::getActivity(), CCRN_CLASSNAME, "setLoadingPercent", (int)(percent * 100.0f));
#endif
}

void Game::setProgressBarAndTextVisible(bool visible)
{
#if (CC_PLATFORM == CC_PLATFORM_IOS)
    CCLoadingWrapper::setProgressBarAndTextVisible(visible);
#endif
#if (CC_PLATFORM == CC_PLATFORM_ANDROID)
    cc::JniHelper::callObjectVoidMethod(cc::JniHelper::getActivity(), CCRN_CLASSNAME, "setProgressBarAndTextVisible", visible);
#endif
}

void Game::setLoadingVisible(bool visible)
{
#if (CC_PLATFORM == CC_PLATFORM_IOS)
    CCLoadingWrapper::setLoadingVisible(visible);
#endif
#if (CC_PLATFORM == CC_PLATFORM_ANDROID)
    cc::JniHelper::callObjectVoidMethod(cc::JniHelper::getActivity(), CCRN_CLASSNAME, "setLoadingVisible", visible);
#endif
}

void Game::onStartGameFailed()
{
#if (CC_PLATFORM == CC_PLATFORM_IOS)
    CCLoadingWrapper::onStartGameFailed();
#endif
#if (CC_PLATFORM == CC_PLATFORM_ANDROID)
    cc::JniHelper::callObjectVoidMethod(cc::JniHelper::getActivity(), CCRN_CLASSNAME, "onStartGameFailed");
#endif
}

#if (CC_PLATFORM == CC_PLATFORM_ANDROID)

extern "C"
{
    JNIEXPORT void JNICALL Java_com_ccrn_CCRNActivity_startGame(JNIEnv *env, jobject activity, jstring game_id, jstring game_data)
    {
        auto app = std::static_pointer_cast<Game>(CC_APPLICATION_MANAGER()->getCurrentApp());
        app->startGame(cc::JniHelper::jstring2string(game_id), cc::JniHelper::jstring2string(game_data));
    }

    JNIEXPORT void JNICALL Java_com_ccrn_CCRNActivity_restartGame(JNIEnv *env, jobject activity)
    {
        auto app = std::static_pointer_cast<Game>(CC_APPLICATION_MANAGER()->getCurrentApp());
        app->restart();
    }

    JNIEXPORT void JNICALL Java_com_ccrn_NativeBridge_onGameLoaded(JNIEnv *env, jobject activity)
    {
        auto app = std::static_pointer_cast<Game>(CC_APPLICATION_MANAGER()->getCurrentApp());
        app->onGameLoaded();
    }
}

#endif

CC_REGISTER_APPLICATION(Game);
