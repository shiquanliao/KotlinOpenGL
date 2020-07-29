//
// Created by stone on 2020/7/21.
//

#ifndef KOTLINOPENGL_XLOG_H
#define KOTLINOPENGL_XLOG_H

#define TOPIC "OPENGL_LEARN"

#ifdef ANDROID

#include <android/log.h>

#define XLOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TOPIC, __VA_ARGS__)
#define XLOGI(...) __android_log_print(ANDROID_LOG_INFO, TOPIC, __VA_ARGS__)
#define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR, TOPIC, __VA_ARGS__)
#else
#define XLOGD(...) printf(TOPIC, __VA_ARGS__)
#define XLOGI(...) printf(TOPIC, __VA_ARGS__)
#define XLOGE(...) printf(TOPIC, __VA_ARGS__)

#endif


#endif //KOTLINOPENGL_XLOG_H
