// name and student number: Claire Feng (260899684)
#include "WireCube.h"

using namespace std;

WireCube::WireCube(const GLuint aPosLocation)
{
	// Create a buffers for doing some quad drawing
	glGenVertexArrays(1, &wireVAO);
	glBindVertexArray(wireVAO);

	glGenBuffers(1, &posBufWireID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufWireID);
	glEnableVertexAttribArray(aPosLocation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
}

WireCube::~WireCube()
{
}

void WireCube::draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV) {
	glBindVertexArray(wireVAO);

	glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	glUniform3f(program->getUniform("col"), 1, 1, 1);
	glUniform1i(program->getUniform("enableLighting"), false);

	// Bind position buffer
	int aPosID = program->getAttribute("aPos");
	glEnableVertexAttribArray(aPosID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufWireID);
	glVertexAttribPointer(aPosID, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	// Draw
	int count = 24; // number of indices to be rendered
	glDrawArrays(GL_LINES, 0, count);
	glUniform1i(program->getUniform("enableLighting"), true);

	glDisableVertexAttribArray(aPosID);
	glBindVertexArray(0);
	GLSL::checkError(GET_FILE_LINE);
}
