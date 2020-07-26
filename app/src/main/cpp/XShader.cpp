//
// Created by stone on 2020/7/24.
//

#include "XShader.h"
#include "XLog.h"
#include <GLES3/gl3.h>
#include <ctime>


#define GET_STR(x) #x

const char *vertexShaderSource = "#version 300 es\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 300 es\n"
                                   "precision mediump float;\n"
                                   "uniform vec4 ourColor;\n"
                                   "out vec4 fragmentColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   fragmentColor = ourColor;\n"
                                   "}\n\0";


static int64_t getCurrentLocalTimeStamp(){
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp =
            std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    return tmp.count();
}

bool XShader::Init() {
    Close();
    std::lock_guard<std::mutex> lockGuard(g_mutex);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
            0.0f, 0.5f, 0.0f  // top
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          nullptr); // first parmeter is vertexShader glsl(layout(location = 0) '0')
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // -------------------------- handle VBO end  --------------------



    // -------------------------- Compiling a shader start --------------------
    // 4. like step1, get the xxxShader ID
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 5. attach the source code to the xxxShader
    glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);

    // 6. compile the vertexShader
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    // check
    // Q: 'iv' at the end of glGetShaderiv() stand for?
    // A: It describes the parameters returned, in this case a “vector” of "ints"
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        XLOGE("vertexShader compile failed:[%s]", infoLog);
        return false;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        XLOGE("fragmentShader compile failed:[%s]", infoLog);
        return false;
    }
    // -------------------------- Compiling a shader end   --------------------


    // -------------------------- shader program start --------------------
    // 7. get a shader program
    shaderProgram = glCreateProgram();

    // 8. attach the shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // 9. link them
    glLinkProgram(shaderProgram);

    // check
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        XLOGE("shaderProgram link failed:[%s]", infoLog);
        return false;
    }

    // 10. user program
    glUseProgram(shaderProgram);
    // -------------------------- shader program end   --------------------


    // 11. release shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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

    // change color
    double greenValue = sin(startTime++ / 30.0f) / 2.0f + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation,0.0f, greenValue, 0.0f, 1.0f);
//    XLOGE("------------------------: %f",greenValue);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
