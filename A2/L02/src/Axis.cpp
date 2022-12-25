// name and student number: Claire Feng (260899684)
#include "Axis.h"

using namespace std;

Axis::Axis(const GLuint aPosLocation) {
	// Create a buffers for doing some line drawing
	glGenVertexArrays(1, &frameVAO);
	glBindVertexArray(frameVAO);
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(aPosLocation);
	glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindVertexArray(0);
}

Axis::~Axis() { }

void Axis::draw(const shared_ptr<Program> program,shared_ptr <MatrixStack> M) {
	glBindVertexArray(frameVAO);

	glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);

	// Bind position buffer
	int aPosID = program->getAttribute("aPos");
	glEnableVertexAttribArray(aPosID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(aPosID, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	//draw frame
	glUniform1i(program->getUniform("enableLighting"), false);

	////TODO: draw axis like in A1
    glUniform3f(program->getUniform("col"), 1, 0, 0);  // red x-axis
    glDrawArrays(GL_LINE_LOOP, 0, 2);
    glUniform3f(program->getUniform("col"), 0, 1, 0);  // green y-axis
    glDrawArrays(GL_LINE_LOOP, 2, 2);
    glUniform3f(program->getUniform("col"), 0, 0, 1);  // blue z-axis
    glDrawArrays(GL_LINE_LOOP, 4, 2);
    

	glUniform1i(program->getUniform("enableLighting"), true);


	glDisableVertexAttribArray(aPosID);
	glBindVertexArray(0);

	GLSL::checkError(GET_FILE_LINE);
}
