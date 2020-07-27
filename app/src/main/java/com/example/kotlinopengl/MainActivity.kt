package com.example.kotlinopengl

import android.content.res.AssetManager
import android.graphics.Point
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.Surface
import com.example.kotlinopengl.JNIUtils.stringFromJNI
import com.example.kotlinopengl.views.NativeSurfaceView
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Example of a call to a native method
//        sample_text.text = stringFromJNI()
//        var point = Point()
//        windowManager.defaultDisplay.getSize(point)
//        nativeSurfaceView.holder.setFixedSize(100,100)
        JNIUtils.init(assets)
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */


    companion object {
        // Used to load the 'native-lib' library on application startup.

    }
}

object JNIUtils {

    init {
        System.loadLibrary("native-lib")
    }

    external fun init(assetManager: AssetManager)
    external fun stringFromJNI(): String

    external fun nativeSurfaceCreate(surfaceView: Surface)

    external fun nativeSurfaceChanged(width: Int, height: Int)

    external fun nativeSurfaceDestroyed()
}
