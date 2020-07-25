//
// Created by stone on 2020/7/24.
//

#include "XShader.h"
#include "XLog.h"
#include <GLES3/gl3.h>

#define GET_STR(x) #x


const char *vertexShaderSource = "#version 300 es\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 300 es\n"
                                   "precision mediump float;\n"
                                   "out vec4 fragmentColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   fragmentColor = vec4(1.0f, 0.5f, 0.2f, 1.0f) ;\n"
                                   "}\n\0";


bool XShader::Init() {
    Close();
    std::lock_guard<std::mutex> lockGuard(g_mutex);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            0.5f, 0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f, 0.5f, 0.0f   // top left
    };
    unsigned int indices[] = { // note that we start from 0!
            0, 1, 3,    // first triangle
            1, 2, 3     // second triangle
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          nullptr); // first parmeter is vertexShader glsl(layout(location = 0) '0')
    glEnableVertexAttribArray(0);


    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(GL_FRONT_AND_BACK, GL_LINES);
    // -------------------------- handle VBO end  --------------------



    // -------------------------- Compiling a shader start --------------------
    // 4. like step1, get the xxxShader ID
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 5. attach the source code to the xxxShader
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

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
    if (EBO)
        glDeleteBuffers(1, &EBO);
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
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
