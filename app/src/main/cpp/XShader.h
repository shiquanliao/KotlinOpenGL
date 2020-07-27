//
// Created by stone on 2020/7/24.
//

#ifndef KOTLINOPENGL_XSHADER_H
#define KOTLINOPENGL_XSHADER_H

#include <mutex>
#include <shader_s.h>

typedef struct XYOffSet{
    float x;
    float y;
} XYOffSet;

class XShader {
public:
    virtual bool Init(std::string &vertexCode, std::string &fragmentCode);

    virtual void Close();

    virtual void Draw();

    virtual void setOffset(XYOffSet& offSet);

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    XYOffSet xyOffSet{0.0f, 0.0f};
    Shader *shader = nullptr;
    std::mutex g_mutex;
};


#endif //KOTLINOPENGL_XSHADER_H
