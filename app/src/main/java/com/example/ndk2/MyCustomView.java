package com.example.ndk2;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.View;

public class MyCustomView extends View {
    private Paint paint;
    
    public MyCustomView(Context context) {
        super(context);
        init();
    }
    
    private void init() {
        paint = new Paint();
        paint.setColor(Color.BLUE);
        paint.setStyle(Paint.Style.FILL);
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        
        // Draw a simple circle
        int x = getWidth() / 2;
        int y = getHeight() / 2;
        int radius = Math.min(getWidth(), getHeight()) / 4;
        
        canvas.drawColor(Color.WHITE);
        canvas.drawCircle(x, y, radius, paint);
    }
}