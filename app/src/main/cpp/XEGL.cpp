//
// Created by stone on 2020/7/21.
//
#include <android/native_window.h>
#include <EGL/egl.h>
#include <mutex>
#include "XLog.h"
#include "XEGL.h"


class CXEGL : public XEGL {
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
    std::mutex g_mutex;

    void Draw() override {
        std::lock_guard<std::mutex> lockGuard(g_mutex);
        eglSwapBuffers(display, surface);
    }

    void Close() override {
        std::lock_guard<std::mutex> lockGuard(g_mutex);
        if (display == EGL_NO_DISPLAY) return;

        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (surface != EGL_NO_SURFACE)
            eglDestroySurface(display, surface);
        if (context != EGL_NO_CONTEXT)
            eglDestroyContext(display, context);

        eglTerminate(display);

        display = EGL_NO_DISPLAY;
        surface = EGL_NO_SURFACE;
        context = EGL_NO_CONTEXT;
    }

    bool Init(void *win) override {
        Close();
        auto *pNativeWindow = static_cast<ANativeWindow *>(win);
        // 1. init EGL
        std::lock_guard<std::mutex> lockGuard(g_mutex);
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) {
            XLOGE("eglGetDisplay failed!");
            return false;
        }
        XLOGI("eglGetDisplay success!");

        // 2. init display
        int major, minor, ret;
        if (EGL_TRUE != eglInitialize(display, &major, &minor)) {
            XLOGE("eglInitialize failed");
            return false;
        }
        XLOGI("eglInitialize success, major is:%d, minor is:%d", major, minor);

        // 3. get config and create surface
        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };
        EGLint config = 0;
        EGLint numConfigs = 0;
        if (EGL_TRUE != eglChooseConfig(display, configSpec, reinterpret_cast<EGLConfig *>(&config), 1,& numConfigs)) {
            XLOGE("eglChooseConfig failed");
            return false;
        }
        XLOGI("eglChooseConfig success! config is:%d, numconfigs is:%d", config, numConfigs);
        surface = eglCreateWindowSurface(display, reinterpret_cast<EGLConfig>(config), pNativeWindow, nullptr);

        // 4. create context
        const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(display, reinterpret_cast<EGLConfig>(config), EGL_NO_CONTEXT, ctxAttr);
        if (context == EGL_NO_CONTEXT) {
            XLOGE("eglCreateContext failed");
            return false;
        }

        // 5. make current
        if (EGL_TRUE != eglMakeCurrent(display, surface, surface, context)) {
            XLOGE("eglMakeCurrent failed");
            return false;
        }

        XLOGI("eglMakeCurrent success!");
        return true;
    }
};


XEGL *XEGL::Get() {
    static CXEGL egl;
    return &egl;
}
