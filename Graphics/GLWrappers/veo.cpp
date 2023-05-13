#include "veo.h"
#include <iostream>

using namespace GraphicsEngine;

VEO::VEO(std::vector<int> data):
    m_length(data.size())
{
    glGenBuffers(1, &m_handle);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data.data(), GL_STATIC_DRAW);
    unbind();
}

VEO::~VEO(){
    glDeleteBuffers(1, &m_handle);
}

void VEO::bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
}

void VEO::unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLsizei VEO::getLength(){
    return m_length;
}
