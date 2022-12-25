// name and student number: Claire Feng (260899684)
#include "MatrixStack.h"

#include <stdio.h>
#include <cassert>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

MatrixStack::MatrixStack()
{
	mstack = make_shared< stack<glm::mat4> >();
	mstack->push(glm::mat4(1.0));
	minvstack = make_shared< stack<glm::mat4> >();
	minvstack->push(glm::mat4(1.0));
}

MatrixStack::~MatrixStack()
{
}

void MatrixStack::pushMatrix()
{
	const glm::mat4 &top = mstack->top();
	const glm::mat4 &topinv = minvstack->top();
	mstack->push(top);
	minvstack->push(topinv);
	assert(mstack->size() < 100);
}

void MatrixStack::popMatrix()
{
	assert(!mstack->empty());
	mstack->pop();
	minvstack->pop();
	// There should always be one matrix left.
	assert(!mstack->empty());
}

void MatrixStack::loadIdentity()
{
	glm::mat4 &top = mstack->top();
	top = glm::mat4(1.0);
	glm::mat4& topinv = minvstack->top();
	topinv = glm::mat4(1.0);
}

void MatrixStack::translate(const glm::vec3 &t)
{
	glm::mat4 &top = mstack->top();
	top *= glm::translate(glm::mat4(1.0f), t);
	glm::mat4& topinv = minvstack->top();
	topinv *= glm::transpose(glm::translate(glm::mat4(1.0f), -t));
}

void MatrixStack::translate(float x, float y, float z)
{
	translate(glm::vec3(x, y, z));
}

void MatrixStack::scale(const glm::vec3 &s)
{
	glm::mat4 &top = mstack->top();
	top *= glm::scale(glm::mat4(1.0f), s);
	
	glm::mat4& topinv = minvstack->top();
	glm::vec3 invs = glm::vec3(1/s.x,1/s.y,1/s.z);
	topinv *= glm::scale(glm::mat4(1.0f), invs);// transpose is the same for a scaling matrix
}

void MatrixStack::scale(float x, float y, float z)
{
	scale(glm::vec3(x, y, z));
}

void MatrixStack::scale(float s)
{
	scale(glm::vec3(s, s, s));
}

void MatrixStack::rotate(float angle, const glm::vec3 &axis)
{
	glm::mat4 &top = mstack->top();
	top *= glm::rotate(glm::mat4(1.0f), angle, axis);

	glm::mat4& topinv = minvstack->top();
	topinv *= glm::transpose(glm::rotate(glm::mat4(1.0f), -angle, axis));
}

void MatrixStack::rotate(float angle, float x, float y, float z)
{
	rotate(angle, glm::vec3(x, y, z));
}

void MatrixStack::multMatrix(const glm::mat4 &matrix)
{
	glm::mat4 &top = mstack->top();
	top *= matrix;
	glm::mat4& topinv = minvstack->top();
	topinv *= glm::transpose(glm::inverse(matrix)); // should never be called, but adding it here for completion's sake
}

const glm::mat4 &MatrixStack::topMatrix() const
{
	return mstack->top();
}

const glm::mat4& MatrixStack::topInvMatrix() const
{
	return minvstack->top();
}

void MatrixStack::print(const glm::mat4 &mat, const char *name)
{
	if(name) {
		printf("%s = [\n", name);
	}
	for(int i = 0; i < 4; ++i) {
		for(int j = 0; j < 4; ++j) {
			// mat[j] returns the jth column
			printf("%- 5.2f ", mat[j][i]);
		}
		printf("\n");
	}
	if(name) {
		printf("];");
	}
	printf("\n");
}

void MatrixStack::print(const char *name) const
{
	print(mstack->top(), name);
}
