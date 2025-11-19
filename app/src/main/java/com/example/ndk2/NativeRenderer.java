package com.example.ndk2;

// OpenGLDemoView.java
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.content.res.AssetManager;
import android.content.Context;
import android.util.Log;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;

public class NativeRenderer implements GLSurfaceView.Renderer {
    static {
        System.loadLibrary("native-lib"); // 加载 C++ 库
    }

    private Context context;

    public NativeRenderer(Context context) {
        this.context = context;
    }

    // Native 方法声明
    public native void nativeSurfaceCreated();
    public native void nativeSurfaceChanged(int width, int height);
    public native void nativeDrawFrame();
    public native void initAssetManager(AssetManager assetManager, String internalPath);

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String filesDir = context.getFilesDir().getAbsolutePath();
        
        // 首次启动时复制 assets 到私有目录
        if (!isAssetsCopied(filesDir)) {
            Log.i("NativeRenderer", "First launch detected, copying assets...");
            copyAllAssets(context.getAssets(), filesDir);
            markAssetsCopied(filesDir);
        } else {
            Log.i("NativeRenderer", "Assets already copied, skipping...");
        }
        
        AssetManager assetManager = context.getAssets();
        // 设置资源路径，但不初始化渲染器（等待 onSurfaceChanged 获得正确的尺寸）
        initAssetManager(assetManager, filesDir);
    }
    
    // 检查 assets 是否已复制
    private boolean isAssetsCopied(String filesDir) {
        File flagFile = new File(filesDir, ".assets_copied");
        return flagFile.exists();
    }
    
    // 标记 assets 已复制
    private void markAssetsCopied(String filesDir) {
        File flagFile = new File(filesDir, ".assets_copied");
        try {
            flagFile.createNewFile();
            Log.i("NativeRenderer", "Assets copy flag created");
        } catch (IOException e) {
            Log.e("NativeRenderer", "Failed to create flag file", e);
        }
    }
    
    // 复制所有 assets 到私有目录
    private void copyAllAssets(AssetManager assetManager, String destPath) {
        long startTime = System.currentTimeMillis();
        try {
            copyAssetsRecursive(assetManager, "", destPath);
            long duration = System.currentTimeMillis() - startTime;
            Log.i("NativeRenderer", "Assets copied successfully in " + duration + "ms");
        } catch (IOException e) {
            Log.e("NativeRenderer", "Failed to copy assets", e);
        }
    }
    
    // 递归复制 assets 目录
    private void copyAssetsRecursive(AssetManager assetManager, String assetPath, String destPath) throws IOException {
        String[] files = assetManager.list(assetPath);
        
        if (files == null || files.length == 0) {
            // 是文件，复制它
            copyAssetFile(assetManager, assetPath, destPath);
        } else {
            // 是目录，创建它并递归
            File dir = new File(destPath);
            if (!dir.exists()) {
                if (dir.mkdirs()) {
                    Log.d("NativeRenderer", "Created directory: " + destPath);
                }
            }
            
            for (String filename : files) {
                String subAssetPath = assetPath.isEmpty() ? filename : assetPath + "/" + filename;
                String subDestPath = destPath + "/" + filename;
                copyAssetsRecursive(assetManager, subAssetPath, subDestPath);
            }
        }
    }
    
    // 复制单个文件
    private void copyAssetFile(AssetManager assetManager, String assetPath, String destPath) throws IOException {
        File destFile = new File(destPath);
        
        // 复制文件
        InputStream in = assetManager.open(assetPath);
        FileOutputStream out = new FileOutputStream(destFile);
        
        byte[] buffer = new byte[8192];
        int bytesRead;
        long totalBytes = 0;
        while ((bytesRead = in.read(buffer)) != -1) {
            out.write(buffer, 0, bytesRead);
            totalBytes += bytesRead;
        }
        
        in.close();
        out.close();
        
        Log.d("NativeRenderer", "Copied: " + assetPath + " (" + totalBytes + " bytes)");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.i("NativeRenderer", "Surface changed: " + width + "x" + height);
        nativeSurfaceChanged(width, height); // 通知 C++ 尺寸变化和初始化渲染器
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        nativeDrawFrame(); // 调用 C++ 渲染
    }
}