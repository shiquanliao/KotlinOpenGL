//
// Created by stone on 2020/7/26.
//

#ifndef KOTLINOPENGL_SHADER_S_H
#define KOTLINOPENGL_SHADER_S_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GLES3/gl3.h>
#include <cstring>
#include "../XLog.h"
#include <glm/glm.hpp>

enum ShaderType {
    VERTEX, FRAGMENT, PROGRAM
};

class Shader {
public:
    // constructor generate the shader on the fly.
    // -------------------------------------------
    Shader(const char *vertexPath, const char *fragmentPath) {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffers contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderStream.rdbuf();

            // close the file handlers
            vShaderFile.close();
            fShaderFile.close();

            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure &e) {
            XLOGE("Shader file not successfully read");
        }
        createProgram(vertexCode, fragmentCode);
    }

    Shader(std::string &vertexCode, std::string &fragmentCode) {
        createProgram(vertexCode, fragmentCode);
    }

    // activate the shader
    // -------------------
    void use() {
        glUseProgram(ID);
    }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    ~Shader() {
        if (ID)
            glDeleteProgram(ID);
    }

private:
    // the program ID
    unsigned int ID;
public:
    unsigned int getId() const {
        return ID;
    }

private:

    void clearChar(std::string &str) {
        std::string temp;
        for (auto character : str) {
            if (character != '\r') temp.append(reinterpret_cast<const char *>(&character));
        }
        str = temp;
    }

    void createProgram(std::string &vertexCode, std::string &fragmentCode) {

//        clearChar(vertexCode);
//        clearChar(fragmentCode);
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, ShaderType::VERTEX);
        // fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, ShaderType::FRAGMENT);
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, ShaderType::PROGRAM);
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }


    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, ShaderType type) {
        int success;
        char infoLog[1024];
        if (type == ShaderType::VERTEX) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (success == GL_FALSE) {
                glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
                XLOGE("vertexShader compile failed:[%s]", infoLog);
            }
        }

        if (type == ShaderType::FRAGMENT) {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (success == GL_FALSE) {
                glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
                XLOGE("fragmentShader compile failed:[%s]", infoLog);
            }
        }

        if (type == ShaderType::PROGRAM) {
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if (success == GL_FALSE) {
                glGetProgramInfoLog(ID, sizeof(infoLog), nullptr, infoLog);
                XLOGE("shaderProgram link failed:[%s]", infoLog);
            }
        }
    }

};

#endif //KOTLINOPENGL_SHADER_S_H
