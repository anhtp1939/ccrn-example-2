package com.ccrn;

import android.app.Activity;
import android.util.Log;

import com.cocos.lib.CocosHelper;
import com.cocos.lib.JsbBridge;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class NativeBridge
{
    public native void onGameLoaded();

    private static String EVENT_STOP_GAME = "EVENT_STOP_GAME";
    private static String EVENT_START_GAME = "EVENT_START_GAME";
    private static String EVENT_GAME_LOADED = "EVENT_GAME_LOADED";

    private static NativeBridge _instance;
    private CCRNActivity _activity = null;
    private NativeBridgeDelegate _delegate;
    private boolean _isInGame = false;

    public static NativeBridge getInstance() {
        if (_instance == null) {
            _instance = new NativeBridge();
        }
        return _instance;
    }

    private NativeBridge() {
        _isInGame = false;
    }

    public void startGame(CCRNActivity activity) {
        _isInGame = true;
        _activity = activity;
        JsbBridge.setCallback(new JsbBridge.ICallback() {
            @Override
            public void onScript(String arg0, String arg1) {
                onScriptEvent(arg0, arg1);
            }
        });
    }

    public void stopGame() {
        _isInGame = false;
        _activity = null;
        JsbBridge.setCallback(null);
    }

    private void onScriptEvent(String eventName, String eventData) {
        Log.i("NativeBridge", "<onScriptEvent> eventName = " + eventName + ", eventData = " + eventData);
        if (eventName.equals(EVENT_STOP_GAME)) {
            if (_activity != null) {
                _activity.stopGame();
            }
            return;
        }
        if (eventName.equals(EVENT_GAME_LOADED)) {
            onGameLoaded();
            return;
        }
        if (this._delegate != null) {
            this._delegate.onScriptEvent(eventName, eventData);
        }
    }

    public void dispatchEventToScript(String eventName, String eventData) {
        if (!_isInGame) {
            Log.w("NativeBridge", "[NativeBridge] <dispatchEventToScript> failed, the engine has not init yet!!!!");
            return;
        }
        Log.i("NativeBridge", "[NativeBridge] <dispatchEventToScript> " + eventName + " " + eventData);
        JsbBridge.sendToScript(eventName, eventData);
    }

    public void setDelegate(NativeBridgeDelegate delegate) {
        this._delegate = delegate;
    }

    public void onGameStartFailed(String reason) {
        if (this._delegate != null) {
            this._delegate.onGameStartFailed(reason);
        }
    }
}
