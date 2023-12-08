package com.ccrn;

public interface NativeBridgeDelegate {
    void onScriptEvent(String eventName, String eventData);
    void onGameStartFailed(String reason);
}
