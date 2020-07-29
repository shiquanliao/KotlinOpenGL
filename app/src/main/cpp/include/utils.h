//
// Created by stone on 2020/7/27.
//

#ifndef KOTLINOPENGL_UTILS_H
#define KOTLINOPENGL_UTILS_H

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <string>
#include "../XLog.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void extractFile(AAssetManager *aAssetManager, const char *fileName, std::string &fileString) {
    AAsset *aAsset = AAssetManager_open(aAssetManager, fileName, AASSET_MODE_UNKNOWN);
    if (aAsset == nullptr) {
        XLOGE("extractFile AAsset is null");
        return;
    }
    off_t bufferSize = AAsset_getLength(aAsset);
    char *buffer = (char *) malloc(bufferSize + 1);

    AAsset_read(aAsset, buffer, bufferSize);
    buffer[bufferSize] = 0;
    fileString = buffer;
    free(buffer);
    XLOGI("assets file content is: %s", fileString.c_str());

    AAsset_close(aAsset);
}

void getTextureInfo(AAssetManager *aAssetManager, const char *fileName, int *w, int *h, int *nr,
                    unsigned char **buffer) {
    AAsset *aAsset = AAssetManager_open(aAssetManager, fileName, AASSET_MODE_UNKNOWN);
    if(aAsset == nullptr){
        XLOGE("getTextureInfo AAsset is null");
        return;
    }
    off_t bufferSize = AAsset_getLength(aAsset);
    unsigned char *fileData = (unsigned char *) AAsset_getBuffer(aAsset);
    *buffer = stbi_load_from_memory(fileData, bufferSize, w, h, nr, 0);
    return;
}

void releaseTextureInfo(unsigned char **buffer){
    stbi_image_free(*buffer);
}

#endif //KOTLINOPENGL_UTILS_H
