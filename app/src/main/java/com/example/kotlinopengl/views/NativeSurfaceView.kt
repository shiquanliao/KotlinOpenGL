package com.example.kotlinopengl.views

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.example.kotlinopengl.JNIUtils
import kotlin.math.log


class NativeSurfaceView(context: Context, attrs: AttributeSet?, defStyleAttr: Int) :
    SurfaceView(context, attrs, defStyleAttr), SurfaceHolder.Callback {

    private val TAG: String = "NativeSurfaceView"
    var startX: Float = 0F;
    var startY: Float = 0F;
    var viewWidth: Int = 0;
    var viewHeight: Int = 0;


    constructor(context: Context) : this(context, null, 0)

    constructor(context: Context, attrs: AttributeSet?) : this(context, attrs, 0)

    init {
        holder.addCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        Log.i(TAG, "surfaceChanged: ")
        viewWidth = width
        viewHeight = height
        JNIUtils.nativeSurfaceChanged(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        Log.i(TAG, "surfaceDestroyed: ")
        JNIUtils.nativeSurfaceDestroyed()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        Log.i(TAG, "surfaceCreated: ")
        if (holder != null) {
            JNIUtils.nativeSurfaceCreate(holder.surface)
        }
    }

    override fun performClick(): Boolean {
        return super.performClick()
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        when (event?.action) {
            MotionEvent.ACTION_DOWN -> {
                startX = event.x;
                startY = event.y;
                return true
            }

            MotionEvent.ACTION_MOVE -> {
                val xOffset = (event.x - startX) / viewWidth
                val yOffset = (event.y - startY) / viewHeight
                Log.e(
                    TAG,
                    "x is:${startX}, y is:${startY}, event.x is: ${event.x}, event.y is: ${event.y},xOffset is:${xOffset}, yOffset is:${yOffset}"
                )
                JNIUtils.moveXY(xOffset, -yOffset)
            }
        }
        performClick()
        return super.onTouchEvent(event)
    }
}