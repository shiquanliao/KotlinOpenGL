//
// Created by stone on 2020/7/24.
//

#include "XShader.h"
#include "XLog.h"
#include "shader_s.h"
#include "EGLThread.h"
#include <GLES3/gl3.h>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


static int64_t getCurrentLocalTimeStamp() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
            std::chrono::time_point_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return tmp.count();
}


static double getTime() {

    timeval tv{};
    gettimeofday(&tv, nullptr);
    static long startTime = tv.tv_sec;
    return double(tv.tv_sec - startTime) + (double) tv.tv_usec / 1000000.0;
}


static void glmFuncTest() {
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    XLOGE("glmFuncTest test translate: %f, %f, %f", vec.x, vec.y, vec.z);
}


bool XShader::Init(std::string &vertexCode, std::string &fragmentCode, TextureInfo (&texInfos)[]) {
    Close();
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    glmFuncTest();
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    cubePositions = new glm::vec3[]{
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    // -------------------------- handle VBO start --------------------
    // 1. Just like any object in OpenGL, this buffer has a unique ID corresponding to that buffer,
    // so we can generate one with a buffer ID using the glGenVertexArrays/glGenBuffers function:
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 2. bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    // We can bind the newly created buffer(VBO) to the GL_ARRAY_BUFFER target with the glBindBuffer function:
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 3. copy data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // tell OpenGL how interpret the vertex data
    // configure vertex attributes(s)
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          nullptr); // first parmeter is vertexShader glsl(layout(location = 0) '0')
    glEnableVertexAttribArray(0);

    // color attrubute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          reinterpret_cast<const void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // -------------------------- handle VBO end  --------------------

    // -------------------------- texture1 start  --------------------
    // texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture1 wrapping/filtering options (on the currently bound texture1 object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texInfos[0].width, texInfos[0].height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, texInfos[0].buffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    // texture 2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT);  // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_MIRRORED_REPEAT);
    // load image, create texture and generate mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texInfos[1].width, texInfos[1].height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texInfos[1].buffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    // -------------------------- texture1 end    --------------------

    shader = new Shader(vertexCode, fragmentCode);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shader->use(); // don't forget to activate/use the shader before setting uniforms!
    shader->setInt("texture1", 0);
    shader->setInt("texture2", 1);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            (float) EGLThread::getSurfaceWidth() /
                                            (float) EGLThread::getSurfaceHeight(),
                                            0.1f, 100.0f);
    shader->setMat4("projection", projection);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    return true;
}

void XShader::Close() {
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (shader != nullptr) {
        delete shader;
        shader = nullptr;
    }
    if (cubePositions != nullptr) {
        delete[] cubePositions;
        cubePositions = nullptr;
    }
}

void XShader::Draw() {
    std::lock_guard<std::mutex> lockGuard(g_mutex);
    if (shader == nullptr)
        return;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();
    shader->setFloat("xOffset", xyOffSet.x);
    shader->setFloat("yOffset", xyOffSet.y);

    // create/view transformation
    glm::mat4 view = glm::mat4(1.0f);
    float radius = 10.0f;
    float camX = sin(getTime()) * radius;
    float camZ = cos(getTime()) * radius;
//    float camX = 0.0f;
//    float camZ = 3.0f;
    view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 1.0f, 0.0f));
    shader->setMat4("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glBindVertexArray(VAO);

    for (unsigned int i = 0; i < 10; i++) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * (float) i;
        if (i % 3 == 0) {
            angle = (float) getTime() * 25.0f;
        }
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void XShader::setOffset(XYOffSet &offSet) {
    this->xyOffSet = offSet;
}
