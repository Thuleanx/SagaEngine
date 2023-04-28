#pragma once

#include "Engine/_Core/logger.h"
#include "debug.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace GraphicsEngine {
class ShaderLoader {
public:
    static GLuint createShaderProgram(std::vector<GLenum> shaderTypes, std::vector<const char *> filepaths) {
        // Create and compile shaders
        std::vector<GLuint> shaderIDs;
        for (int i = 0; i < shaderTypes.size(); i++) {
            shaderIDs.push_back(createAndCompileShader(shaderTypes[i], filepaths[i]));
        }

        // Link the shader program
        GLuint programID = glCreateProgram();
        for (int i = 0; i < shaderIDs.size(); i++) {
            glAttachShader(programID, shaderIDs[i]);
        }
        glLinkProgram(programID);

        // Print the info log if program fails to link
        GLint status;
        glGetProgramiv(programID, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, '\0');
            glGetProgramInfoLog(programID, length, nullptr, &log[0]);
            glDeleteProgram(programID);
            throw std::runtime_error(log);
        }

        // Individual shaders no longer necessary, stored in program
        for (int i = 0; i < shaderIDs.size(); i++) {
            glDeleteShader(shaderIDs[i]);
        }

        SINFO("END OF SHADERLOADER");

        return programID;
    }

private:
    static std::string readFile(const char *filePath) {
        SINFO("READFILE %s", filePath);
        std::string content;
        std::ifstream fileStream(filePath, std::ios::in);

        if (!fileStream.is_open()) {
            std::string log = "Could not read file " + std::string(filePath) + ". File does not exist.";
            SERROR(log.c_str());
            throw std::runtime_error(log);
            return "";
        }

        std::string line = "";
        while (std::getline(fileStream, line)) {
            content.append(line + "\n");
        }

        fileStream.close();

        return content;
    }

    static GLuint createAndCompileShader(GLenum shaderType, const char *filepath) {
        SINFO("Create and Compile Shader: %s", filepath);
        GLuint shaderID = glCreateShader(shaderType);

        // Read shader file
        std::string shaderStr = readFile(filepath);
        STRACE("Successfully read file %s", filepath);
        const char *shaderSrc = shaderStr.c_str();

        SINFO("GL SHADER SOURCE");
        glShaderSource(shaderID, 1, &shaderSrc, NULL);
        STRACE("Successfully sourced file %s", filepath);

        SINFO("GL COMPILE SHADER: %d", shaderID);
        glCompileShader(shaderID);


        // Print info log if shader fails to compile
        GLint status;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
            std::string log(length, '\0');
            glGetShaderInfoLog(shaderID, length, nullptr, &log[0]);
            glDeleteShader(shaderID);
            SERROR("Shader failed to compile: %s", log.c_str());
            throw std::runtime_error(log);
        }

        STRACE("Successfully compiled file %s", filepath);

        return shaderID;
    }
};
} // namespace GraphicsEngine
