#include "shape.h"
#include <iostream>

using namespace GraphicsEngine;

Shape::Shape(std::shared_ptr<VAO> vao):
    m_vao(vao)
{

}

Shape::~Shape(){

}

void Shape::draw(){
    m_vao->draw();
}