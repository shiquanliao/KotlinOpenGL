//
// Created by stone on 2020/7/24.
//

#ifndef KOTLINOPENGL_XSHADER_H
#define KOTLINOPENGL_XSHADER_H

#include <mutex>

class XShader {
public:
    virtual bool Init();

    virtual void Close();

    virtual void Draw();

private:
    unsigned int VAO[2]{};
    unsigned int VBO[2]{};
    unsigned int EBO = 0;
    unsigned int vertexShader = 0;
    unsigned int fragmentShader = 0;
    unsigned int shaderProgram = 0;
    std::mutex g_mutex;
};


#endif //KOTLINOPENGL_XSHADER_H
