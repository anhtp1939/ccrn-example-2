package com.ccrn;

import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.ccrnexample2.R;
import com.cocos.lib.CocosActivity;
import com.cocos.lib.CocosHelper;
import com.cocos.service.SDKWrapper;

public class CCRNActivity extends CocosActivity {
    public native void startGame(String gameId, String gameData);
    public native void restartGame();

    private FrameLayout frameLayout;
    private ImageView logoImageView;
    private ProgressBar progressBar;
    private TextView loadingText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SDKWrapper.shared().init(this);
        this.initLoadingUI();
        startCurrentGameAfter(500);
    }

    private void startCurrentGameAfter(int delay) {
        final Handler handler = new Handler(Looper.getMainLooper());
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                startCurrentGame();
            }
        }, delay);
    }

    private void startCurrentGame() {
        CocosHelper.runOnGameThread(new Runnable() {
            @Override
            public void run() {
                startGame(CCRNModule.GAME_ID, CCRNModule.GAME_DATA);
                startGameNativeBridge();
            }
        });
    }

    private void startGameNativeBridge() {
        NativeBridge.getInstance().startGame(this);
    }

    public void stopGame() {
        restartGame();
        finish();
        NativeBridge.getInstance().stopGame();
    }

    @Override
    protected void onStart() {
        super.onStart();
        SDKWrapper.shared().onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        SDKWrapper.shared().onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        SDKWrapper.shared().onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Workaround in
        // https://stackoverflow.com/questions/16283079/re-launch-of-activity-on-home-button-but-only-the-first-time/16447508
        if (!isTaskRoot()) {
            return;
        }
        SDKWrapper.shared().onDestroy();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        SDKWrapper.shared().onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        SDKWrapper.shared().onNewIntent(intent);
    }

    @Override
    protected void onRestart() {
        super.onRestart();
        SDKWrapper.shared().onRestart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        SDKWrapper.shared().onStop();
    }

    @Override
    public void onBackPressed() {
        SDKWrapper.shared().onBackPressed();
        super.onBackPressed();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        SDKWrapper.shared().onConfigurationChanged(newConfig);
        super.onConfigurationChanged(newConfig);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        SDKWrapper.shared().onRestoreInstanceState(savedInstanceState);
        super.onRestoreInstanceState(savedInstanceState);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        SDKWrapper.shared().onSaveInstanceState(outState);
        super.onSaveInstanceState(outState);
    }

    @Override
    public void onLowMemory() {
        SDKWrapper.shared().onLowMemory();
        super.onLowMemory();
    }

    private void initLoadingUI() {
        frameLayout = findViewById(contentViewId);

        // logo image
        logoImageView = new ImageView(this);
        logoImageView.setImageResource(R.drawable.logo);
        FrameLayout.LayoutParams logoParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.WRAP_CONTENT,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        logoParams.gravity = Gravity.CENTER;
        logoParams.bottomMargin = 250;
        logoImageView.setLayoutParams(logoParams);
        logoImageView.setScaleX(0.5f);
        logoImageView.setScaleY(0.5f);
        frameLayout.addView(logoImageView);

        // progress bar
        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        int screenWidth = displayMetrics.widthPixels;
        int desiredWidth = (int) (0.7 * screenWidth);
        progressBar = new ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal);
        progressBar.setMax(100);
        progressBar.setProgress(0);
        FrameLayout.LayoutParams progressParams = new FrameLayout.LayoutParams(
                desiredWidth,
                FrameLayout.LayoutParams.WRAP_CONTENT);
        progressParams.gravity = Gravity.CENTER;
        progressParams.topMargin = 200;
        progressBar.setLayoutParams(progressParams);
        frameLayout.addView(progressBar);

        // loading text
        loadingText = new TextView(this);
        loadingText.setText("Updating...");
        FrameLayout.LayoutParams textParams = new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.WRAP_CONTENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        );
        textParams.gravity = Gravity.CENTER;
        textParams.topMargin = 300;
        loadingText.setLayoutParams(textParams);
        loadingText.setTextColor(Color.WHITE);
        loadingText.setTextSize(22);
        frameLayout.addView(loadingText);
        loadingText.setVisibility(View.INVISIBLE);

        this.setProgressBarAndTextVisible(false);
    }

    public void setLoadingPercent(int percentage) {
        runOnUiThread(() -> {
            Log.d("CCRNActivity", "percentage = " + percentage);
            progressBar.setProgress(percentage > 100 ? 100 : percentage);
        });
    }

    public void setProgressBarAndTextVisible(boolean visible) {
        runOnUiThread(() -> {
            if (loadingText != null) loadingText.setVisibility(visible ? View.VISIBLE : View.INVISIBLE);
            if (progressBar != null) progressBar.setVisibility(visible ? View.VISIBLE : View.INVISIBLE);
        });
    }

    public void setLoadingVisible(boolean visible) {
        runOnUiThread(() -> {
            if (loadingText != null) loadingText.setVisibility(visible ? View.VISIBLE : View.INVISIBLE);
            if (progressBar != null) progressBar.setVisibility(visible ? View.VISIBLE : View.INVISIBLE);
            if (logoImageView != null) logoImageView.setVisibility(visible ? View.VISIBLE : View.INVISIBLE);
        });
    }

    public void onStartGameFailed() {
        NativeBridge.getInstance().onGameStartFailed("Game Downloaded Failed!");
        this.stopGame();
    }
}