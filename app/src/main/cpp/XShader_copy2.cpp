//
// Created by stone on 2020/7/24.
//

#include "XShader.h"
#include "XLog.h"
#include <GLES3/gl3.h>


//顶点着色器glsl
#define GET_STR(x) #x
static const char *vertexShaderSource = GET_STR(
        attribute vec4 aPosition; //顶点坐标
        attribute vec2 aTexCoord; //材质顶点坐标
        varying vec2 vTexCoord;   //输出的材质坐标
        void main(){
            vTexCoord = vec2(aTexCoord.x,1.0-aTexCoord.y);
            gl_Position = aPosition;
        }
);

//片元着色器,软解码和部分x86硬解码
static const char *fragYUV420P = GET_STR(
        precision mediump float;    //精度
        varying vec2 vTexCoord;     //顶点着色器传递的坐标
        uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;
        void main(){
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture,vTexCoord).r;
            yuv.g = texture2D(uTexture,vTexCoord).r - 0.5;
            yuv.b = texture2D(vTexture,vTexCoord).r - 0.5;
            rgb = mat3(1.0,     1.0,    1.0,
                       0.0,-0.39465,2.03211,
                       1.13983,-0.58060,0.0)*yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb,1.0);
        }
);




bool XShader::Init() {
    Close();
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    //创建shader
    GLuint sh = glCreateShader(GL_VERTEX_SHADER);
    if(sh == 0)
    {
        XLOGE("glCreateShader %d failed!",GL_VERTEX_SHADER);
        return 0;
    }
    //加载shader
    glShaderSource(sh,
                   1,    //shader数量
                   &vertexShaderSource, //shader代码
                   0);   //代码长度
    //编译shader
    glCompileShader(sh);

    //获取编译情况
    GLint status;
    glGetShaderiv(sh,GL_COMPILE_STATUS,&status);
    if(status == 0)
    {
        XLOGE("glCompileShader failed!");
        return 0;
    }
    XLOGE("glCompileShader success!");
    return sh;

    return true;
}

void XShader::Close() {
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (shaderProgram)
        glDeleteProgram(shaderProgram);

}

void XShader::Draw() {
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    if (!shaderProgram)
        return;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
