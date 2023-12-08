package com.ccrn;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.modules.core.DeviceEventManagerModule;

public class CCRNModule extends ReactContextBaseJavaModule implements NativeBridgeDelegate{
    public static String GAME_ID;
    public static String GAME_DATA;

    CCRNModule(ReactApplicationContext context) {
        super(context);
        NativeBridge.getInstance().setDelegate(this);
    }

    @NonNull
    @Override
    public String getName() {
        return "CCRNModule";
    }

    @ReactMethod
    public void startGame(String gameId, String data) {
        GAME_ID = gameId;
        GAME_DATA = data;
        Activity currentActivity = getCurrentActivity();
        Intent intent = new Intent(currentActivity, CCRNActivity.class);
        currentActivity.startActivity(intent);
    }

    @ReactMethod
    public void dispatchEventToReact(String eventName, String eventData) {
        Log.i("CCRNModule", "[CCRNModule] <dispatchEventToReact> " + eventName + ", " + eventData);
        ReactApplicationContext reactContext = this.getReactApplicationContext();
        reactContext.getJSModule(DeviceEventManagerModule.RCTDeviceEventEmitter.class).emit(eventName, eventData);
    }

    @ReactMethod
    public void dispatchEventToCocos(String eventName, String eventData) {
        Log.d("CCRNModule", "[CCRNModule] <dispatchEventToCocos> " + eventName + ", " + eventData);
        NativeBridge.getInstance().dispatchEventToScript(eventName, eventData);
    }

    @Override
    public void onScriptEvent(String eventName, String eventData) {
        this.dispatchEventToReact(eventName, eventData);
    }

    @Override
    public void onGameStartFailed(String reason) {
        this.dispatchEventToReact("ON_START_GAME_FAILED", reason);
    }
}
