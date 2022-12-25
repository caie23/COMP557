#include "DAGNode.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;
class Shape;

DAGNode::DAGNode()
{
}

DAGNode::~DAGNode()
{
}

void DAGNode::init()
{
}

void DAGNode::scaleOffset(float scale) {
    std::vector <DAGNode*> children = this->children;
    for (DAGNode* child : children) {
        child->offset.x *= scale;
        child->offset.y *= scale;
        child->offset.z *= scale;
        child->scaleOffset(scale);
    }
}


void DAGNode::draw(const std::shared_ptr<Program> prog, const std::shared_ptr<MatrixStack> P, const std::shared_ptr<MatrixStack> MV, glm::fvec3 acc_offset, shared_ptr<Shape> shape, float* frameData ) const
{
    std::vector <DAGNode*> children = this->children;
    for (DAGNode* child : children) {
        // accumulate the offset
        float accx = acc_offset.x + child->offset.x;
        float accy = acc_offset.y + child->offset.y;
        float accz = acc_offset.z + child->offset.z;
        
        // Draw cube
        prog->bind();
        MV->pushMatrix();
        MV->scale(0.01, 0.01, 0.01);
        MV->translate(glm::fvec3(accx, accy, accz));
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
        glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
        shape->draw(prog);
        MV->popMatrix();
        prog->unbind();
        child->draw(prog, P, MV, glm::fvec3(accx, accy, accz), shape, frameData);
    }
}
