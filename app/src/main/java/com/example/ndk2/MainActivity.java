package com.example.ndk2;

import androidx.appcompat.app.AppCompatActivity;
import android.app.Activity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.ndk2.databinding.ActivityMainBinding;
import android.util.Log;

import android.opengl.GLSurfaceView;

// public class MainActivity extends AppCompatActivity {

//     // Used to load the 'ndk2' library on application startup.
//     static {
//         System.loadLibrary("ndk2");
//     }

//     private static final String TAG = "MyApp"; // ✅ Java 的正确写法
//     // private ActivityMainBinding binding;

//     // private MyGLSurfaceView glSurfaceView;

//     @Override
//     protected void onCreate(Bundle savedInstanceState) {
//         // super.onCreate(savedInstanceState);
//         // final private  GLSurfaceView glSurfaceView;

//         // binding = ActivityMainBinding.inflate(getLayoutInflater());
//         // setContentView(binding.getRoot());

//         // // Example of a call to a native method
//         // TextView tv = binding.sampleText;
//         // tv.setText(stringFromJNI());
//         // Log.i(TAG, "onCreate: super.onCreate 1");
//         // super.onCreate(savedInstanceState);
//         // Log.i(TAG, "onCreate: super.onCreate 2");
        
//         // // 创建GLSurfaceView实例
//         // glSurfaceView = new MyGLSurfaceView(this);
//         // Log.i(TAG, "onCreate: jjjjjj");
//         // // 设置为Activity的内容视图
//         // setContentView(glSurfaceView);

//         super.onCreate(savedInstanceState);

//         // surface view
//         // setContentView(new MySurfaceView(this));
//         Log.i(TAG, "onCreate: kkkkkkk");

//         // text view
// //        TextView textView = new TextView(this);
// //        textView.setText("Hello, Android!");
// //        textView.setTextSize(20);
// //        textView.setPadding(50, 50, 50, 50);
// //        setContentView(textView);

//         // normal view
//         setContentView(new MyCustomView(this));
//     }

//     // @Override
//     // protected void onPause() {
//     //     Log.i(TAG, "onPause: kkkkkkk");
//     //     super.onPause();
//     //     glSurfaceView.onPause();
//     //     Log.i(TAG, "onPause: kkkkkkk 2");
//     // }

//     // @Override
//     // protected void onResume() {
//     //     Log.i(TAG, "onResume: kkkkkkk 1");
//     //     super.onResume();
//     //     glSurfaceView.onResume();
//     //     Log.i(TAG, "onResume: kkkkkkk 2");
//     // }

//     /**
//      * A native method that is implemented by the 'ndk2' native library,
//      * which is packaged with this application.
//      */
// //    public native String stringFromJNI();
// }

public class MainActivity extends AppCompatActivity {
    private OpenGLDemoView glSurfaceView;
    private boolean rendererSet = false;

//     static {
//         System.loadLibrary("native-lib");
//     }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        glSurfaceView = new OpenGLDemoView(this);
        
        setContentView(glSurfaceView);
        rendererSet = true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (rendererSet) {
            glSurfaceView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (rendererSet) {
            glSurfaceView.onResume();
        }
    }
}