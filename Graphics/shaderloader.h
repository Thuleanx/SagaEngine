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

        const std::string includeKeyWord = "#include ";

        std::string line = "";
        while (std::getline(fileStream, line)) {
            if (line.starts_with(includeKeyWord)) {
                std::string filenameToInclude = line.substr(includeKeyWord.length(), line.length() - includeKeyWord.length());
                content.append(readFile(filenameToInclude.c_str()) + "\n");
            } else {
                content.append(line + "\n");
            }
        }

        fileStream.close();

        return content;
    }

    static GLuint createAndCompileShader(GLenum shaderType, const char *filepath) {
        SINFO("Create and Compile Shader: %s", filepath);
        GLuint shaderID = glCreateShader(shaderType);

        // Read shader file
        std::string shaderStr = readFile(filepath);
        const char *shaderSrc = shaderStr.c_str();

        glShaderSource(shaderID, 1, &shaderSrc, NULL);

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
            SERROR("Shader %s failed to compile: %s", filepath, log.c_str());
            throw std::runtime_error(log);
        }

        STRACE("Successfully compiled file %s", filepath);

        return shaderID;
    }
};
} // namespace GraphicsEngine
