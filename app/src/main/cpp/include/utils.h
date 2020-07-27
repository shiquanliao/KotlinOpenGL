//
// Created by stone on 2020/7/27.
//

#ifndef KOTLINOPENGL_UTILS_H
#define KOTLINOPENGL_UTILS_H

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>
#include "../XLog.h"

void extractFile(AAssetManager* aAssetManager, const char* fileName,std::string &fileString){
    AAsset* aAsset = AAssetManager_open(aAssetManager, fileName, AASSET_MODE_UNKNOWN);
    if (aAsset == nullptr) {
        XLOGE("extractFile AAsset is null");
        return;
    }
    off_t bufferSize = AAsset_getLength(aAsset);
    char * buffer = (char*) malloc(bufferSize + 1);

    AAsset_read(aAsset, buffer, bufferSize);
    buffer[bufferSize] = 0;
    fileString = buffer;
    free(buffer);
    XLOGI("assets file content is: %s" , fileString.c_str());

    AAsset_close(aAsset);
}


#endif //KOTLINOPENGL_UTILS_H
