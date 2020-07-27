//
// Created by stone on 2020/7/22.
//

#include "EGLThread.h"
#include "XLog.h"
#include "RenderManager.h"
#include <unistd.h>


extern void *eglThreadImpl(void *context);

EGLThread::EGLThread() : mEglThread(-1), pthread_mutex{}, pthread_cond{}, m_ANativeWindow(nullptr),
                         isCreate(false), isChange(false), isExit(false), isStart(false),
                         surfaceWidth(0), surfaceHeight(0), renderType(RENDER_MODULE_MANUAL) {
    pthread_mutex_init(&pthread_mutex, nullptr);
    pthread_cond_init(&pthread_cond, nullptr);
}


EGLThread::~EGLThread() {
    pthread_mutex_destroy(&pthread_mutex);
    pthread_cond_destroy(&pthread_cond);
}


void EGLThread::onSurfaceCreate(EGLNativeWindowType window, std::string &vertexCode,
                                std::string &fragmentCode) {
    if (mEglThread == -1) {
        isCreate = true;
        m_ANativeWindow = window;
        m_VertexCode = vertexCode;
        m_FragmentCode = fragmentCode;
        pthread_create(&mEglThread, nullptr, eglThreadImpl, this);
    }
}

void EGLThread::onSurfaceChange(int width, int height) {
    if (mEglThread != -1) {
        isChange = true;
        surfaceWidth = width;
        surfaceHeight = height;
        notifyRender();
    }
}

void EGLThread::setRenderModule(int type) {
    if (type == RENDER_MODULE_AUTO || type == RENDER_MODULE_MANUAL) {
        this->renderType = type;
        notifyRender();
    }

}

void EGLThread::notifyRender() {
    pthread_mutex_lock(&pthread_mutex);
    pthread_cond_signal(&pthread_cond);
    pthread_mutex_unlock(&pthread_mutex);
}

void EGLThread::callBackOnCreate(OnCreate create) {
    this->onCreate = create;
}

void EGLThread::callBackOnChange(OnChange change) {
    this->onChange = change;
}

void EGLThread::callBackOnDraw(OnDraw draw) {
    this->onDraw = draw;
}

void *eglThreadImpl(void *context) {
    auto *eglThread = static_cast<EGLThread *>(context);
    if (!eglThread) {
        XLOGE("eglThreadImpl eglThread is null");
        return 0;
    }

    RenderManager *renderManager = RenderManager::GetInstance();

    if (!renderManager->Init(eglThread->m_ANativeWindow, eglThread->m_VertexCode, eglThread->m_FragmentCode)) {
        XLOGE("XEGL init error");
        return 0;
    }
    eglThread->isExit = false;
    while (!eglThread->isExit) {
        if (eglThread->isCreate) {
            eglThread->isCreate = false;
            eglThread->onCreate();
        }

        if (eglThread->isChange) {
            eglThread->isChange = false;
            eglThread->isStart = true;
            eglThread->onChange(eglThread->surfaceWidth, eglThread->surfaceHeight);
        }

        if (eglThread->isStart) {
            eglThread->onDraw();
            renderManager->Draw();
            if (eglThread->renderType == RENDER_MODULE_AUTO) {
                usleep(1000000 );
            } else {
                pthread_mutex_lock(&eglThread->pthread_mutex);
                pthread_cond_wait(&eglThread->pthread_cond, &eglThread->pthread_mutex);
                pthread_mutex_unlock(&eglThread->pthread_mutex);
            }
        }
    }

    renderManager->Close();
    renderManager = nullptr;

    return nullptr;
}

void EGLThread::setIsExit(bool exit) {
    EGLThread::isExit = exit;
}


