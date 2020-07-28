//
// Created by stone on 2020/7/24.
//

#include "XShader.h"
#include "XLog.h"
#include "shader_s.h"
#include <GLES3/gl3.h>
#include <ctime>


static int64_t getCurrentLocalTimeStamp() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
            std::chrono::time_point_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return tmp.count();
}

bool XShader::Init(std::string &vertexCode, std::string &fragmentCode, TextureInfo &texInfo) {
    Close();
    std::lock_guard<std::mutex> lockGuard(g_mutex);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // colors           // texture coords
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom left
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // top left
    };

    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    // -------------------------- handle VBO start --------------------
    // 1. Just like any object in OpenGL, this buffer has a unique ID corresponding to that buffer,
    // so we can generate one with a buffer ID using the glGenVertexArrays/glGenBuffers function:
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 2. bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    // We can bind the newly created buffer(VBO) to the GL_ARRAY_BUFFER target with the glBindBuffer function:
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // 3. copy data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // tell OpenGL how interpret the vertex data
    // configure vertex attributes(s)
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          nullptr); // first parmeter is vertexShader glsl(layout(location = 0) '0')
    glEnableVertexAttribArray(0);

    // color attrubute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<const void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<const void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // -------------------------- handle VBO end  --------------------

    // -------------------------- texture start  --------------------
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texInfo.width, texInfo.height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, texInfo.buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    // -------------------------- texture end    --------------------

    shader = new Shader(vertexCode, fragmentCode);
    return true;
}

void XShader::Close() {
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (EBO)
        glDeleteBuffers(1, &EBO);
    if (shader != nullptr) {
        delete shader;
        shader = nullptr;
    }
}

void XShader::Draw() {
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    if (shader == nullptr)
        return;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();
    shader->setFloat("xOffset", xyOffSet.x);
    shader->setFloat("yOffset", xyOffSet.y);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);

//    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void XShader::setOffset(XYOffSet &offSet) {
    this->xyOffSet = offSet;
}
