#include <jni.h>
#include <string>
#include <android/asset_manager_jni.h>
#include <utils.h>
#include "EGLThread.h"
#include "XLog.h"
#include "GLES3/gl3.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "pthread.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"


#define VERTEXSHADER "vshader.glsl"
#define FRAGMENTSHADER "fshader.glsl"
#define TEXTURE_IMAGE_WALL "wall.jpg"
#define TEXTURE_IMAGE_CONTAINER "container.jpg"

EGLThread *eglThread = nullptr;
std::string vertexShaderCode;
std::string fragmentShaderCode;
TextureInfo textureInfo{};

void callBackOnCreate() {
//    XLOGE("callBackOnCreate");
}

void callBackOnChange(int width, int height) {
    glViewport(0, 0, width, height);
//    XLOGE("callBackOnChange");
}

void callBackOnDraw() {
//    glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
//    XLOGE("callBackOnDraw");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_kotlinopengl_JNIUtils_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_kotlinopengl_JNIUtils_nativeSurfaceCreate(JNIEnv *env, jobject thiz,
                                                           jobject surface_view) {
    eglThread = new EGLThread();
    eglThread->callBackOnCreate(callBackOnCreate);
    eglThread->callBackOnChange(callBackOnChange);
    eglThread->callBackOnDraw(callBackOnDraw);
    eglThread->setRenderModule(RENDER_MODULE_AUTO);

    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface_view);
    eglThread->onSurfaceCreate(nativeWindow, vertexShaderCode, fragmentShaderCode, textureInfo);
    releaseTextureInfo(&textureInfo.buffer);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_kotlinopengl_JNIUtils_nativeSurfaceChanged(JNIEnv *env, jobject thiz, jint width,
                                                            jint height) {
    if (eglThread) {
        eglThread->onSurfaceChange(width, height);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_kotlinopengl_JNIUtils_nativeSurfaceDestroyed(JNIEnv *env, jobject thiz) {
    if (eglThread) {
        eglThread->setIsExit(true);
        //等待线程结束
        pthread_join(reinterpret_cast<pthread_t>(eglThread), nullptr);
        delete (eglThread);
        eglThread = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kotlinopengl_JNIUtils_init(JNIEnv *env, jobject thiz, jobject asset_manager) {
    AAssetManager *aAssetManager = AAssetManager_fromJava(env, asset_manager);
    extractFile(aAssetManager, VERTEXSHADER, vertexShaderCode);
    extractFile(aAssetManager, FRAGMENTSHADER, fragmentShaderCode);
    getTextureInfo(aAssetManager, TEXTURE_IMAGE_CONTAINER, &textureInfo.width, &textureInfo.height,
                   &textureInfo.nrChannels, &textureInfo.buffer);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kotlinopengl_JNIUtils_moveXY(JNIEnv *env, jobject thiz, jfloat x, jfloat y) {
    eglThread->moveXY(x, y);
    XLOGE("native-lib moveXY: x is:%f, y is:%f", x, y);
}