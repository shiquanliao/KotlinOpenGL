package com.example.kotlinopengl.views

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.kotlinopengl.JNIUtils


class NativeSurfaceView(context: Context, attrs: AttributeSet?, defStyleAttr: Int) :
    SurfaceView(context, attrs, defStyleAttr), SurfaceHolder.Callback {

    constructor(context: Context) : this(context, null, 0)

    constructor(context: Context, attrs: AttributeSet?) : this(context, attrs, 0)

    init {
        holder.addCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        JNIUtils.nativeSurfaceChanged(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        JNIUtils.nativeSurfaceDestroyed()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        if (holder != null) {
            JNIUtils.nativeSurfaceCreate(holder.surface)
        }
    }


}