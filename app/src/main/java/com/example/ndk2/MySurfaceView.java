package com.example.ndk2;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class MySurfaceView extends SurfaceView 
    implements SurfaceHolder.Callback, Runnable {
    
    private SurfaceHolder mHolder;
    private Thread mRenderThread;
    private boolean mIsRunning;
    
    public MySurfaceView(Context context) {
        super(context);
        mHolder = getHolder();
        mHolder.addCallback(this);
    }
    
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mIsRunning = true;
        mRenderThread = new Thread(this);
        mRenderThread.start();
    }
    
    @Override
    public void surfaceChanged(SurfaceHolder holder, 
            int format, int width, int height) {
        // Surface 尺寸变化时调用
    }
    
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mIsRunning = false;
        try {
            mRenderThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    
    @Override
    public void run() {
        while (mIsRunning) {
            Canvas canvas = null;
            try {
                canvas = mHolder.lockCanvas();
                if (canvas != null) {
                    // 在这里进行绘制
                    drawSomething(canvas);
                }
            } finally {
                if (canvas != null) {
                    mHolder.unlockCanvasAndPost(canvas);
                }
            }
        }
    }
    
    private void drawSomething(Canvas canvas) {
        // 示例：绘制一个移动的圆
        canvas.drawColor(Color.WHITE);
        Paint paint = new Paint();
        paint.setColor(Color.GREEN);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(20.0f);
        
        long time = System.currentTimeMillis();
        int x = (int)(getWidth() * (0.5 + 0.3 * Math.sin(time / 500.0)));
        int y = (int)(getHeight() * (0.5 + 0.3 * Math.cos(time / 500.0)));
        int radius = Math.min(getWidth(), getHeight()) / 10;
        
        canvas.drawCircle(x, y, radius, paint);

        canvas.drawCircle(x, y, 30, paint);
    }
}