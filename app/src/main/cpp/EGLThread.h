//
// Created by stone on 2020/7/22.
//

#ifndef KOTLINOPENGL_EGLTHREAD_H
#define KOTLINOPENGL_EGLTHREAD_H

#include <EGL/egl.h>
#include <string>
#include "pthread.h"
#include "XShader.h"


#define RENDER_MODULE_AUTO 1
#define RENDER_MODULE_MANUAL 2


typedef void(*OnCreate)();

typedef void(*OnChange)(int width, int height);

typedef void(*OnDraw)();

class EGLThread {

public:
    friend void *eglThreadImpl(void *context);

    void
    onSurfaceCreate(EGLNativeWindowType window, std::string &vertexCode, std::string &fragmentCode,
                    TextureInfo (&texInfo)[]);

    void onSurfaceChange(int width, int height);

    void setRenderModule(int type);

    void notifyRender();

    void callBackOnCreate(OnCreate create);

    void callBackOnChange(OnChange change);

    void callBackOnDraw(OnDraw draw);

    void moveXY(float x, float y);

    void setIsExit(bool exit);

    static int getSurfaceWidth();

    static int getSurfaceHeight();

    EGLThread();

    ~EGLThread();

private:

    pthread_t mEglThread = -1;
public:
    pthread_t getMEglThread() const;

private:
    pthread_mutex_t pthread_mutex;
    pthread_cond_t pthread_cond;
    ANativeWindow *m_ANativeWindow;
    std::string m_VertexCode;
    std::string m_FragmentCode;
    TextureInfo (*m_TextureInfos)[];
    bool isCreate;
    bool isChange;
    bool isExit;
    bool isStart;
    int renderType;
    static int surfaceWidth;
    static int surfaceHeight;

    XYOffSet xyOffSet{0.0f, 0.0f};

    OnCreate onCreate{};

    OnChange onChange{};

    OnDraw onDraw{};

};


#endif //KOTLINOPENGL_EGLTHREAD_H
