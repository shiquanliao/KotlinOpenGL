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


#define VERTEXSHADER "vshader.glsl"
#define FRAGMENTSHADER "fshader.glsl"
#define TEXTURE_IMAGE_WALL "wall.jpg"
#define TEXTURE_IMAGE_CONTAINER "container.jpg"
#define TEXTURE_IMAGE_FACE "awesomeface.png"

EGLThread *eglThread = nullptr;
std::string vertexShaderCode;
std::string fragmentShaderCode;
TextureInfo tex1{};
TextureInfo tex2{};
TextureInfo textureInfos[] = {tex1, tex2};

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
    eglThread->onSurfaceCreate(nativeWindow, vertexShaderCode, fragmentShaderCode,
                               reinterpret_cast<TextureInfo (&)[]>(textureInfos));
    releaseTextureInfo(&textureInfos[0].buffer);
    releaseTextureInfo(&textureInfos[1].buffer);
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
    getTextureInfo(aAssetManager, TEXTURE_IMAGE_CONTAINER, &textureInfos[0].width, &textureInfos[0].height,
                   &textureInfos[0].nrChannels, &textureInfos[0].buffer);
    getTextureInfo(aAssetManager, TEXTURE_IMAGE_FACE, &textureInfos[1].width, &textureInfos[1].height,
            &textureInfos[1].nrChannels, &textureInfos[1].buffer);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_kotlinopengl_JNIUtils_moveXY(JNIEnv *env, jobject thiz, jfloat x, jfloat y) {
    eglThread->moveXY(x, y);
    XLOGE("native-lib moveXY: x is:%f, y is:%f", x, y);
}