// name and student number: Claire Feng (260899684)
#include "QuadTextured.h"

using namespace std;


QuadTextured::QuadTextured(const GLuint aPosLocation, const GLuint aNorLocation, const GLuint aTexLocation)
{
	// Create a buffers for doing some quad drawing
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	//create the position buffer and fill it with the class's vertices vector
	glGenBuffers(1, &posBufQuadID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufQuadID);
	glEnableVertexAttribArray(aPosLocation);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//create the normal buffer and fill it with the class's normal vector
	glGenBuffers(1, &norBufQuadID);
	glBindBuffer(GL_ARRAY_BUFFER, norBufQuadID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(aNorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//create the texture coordinate buffer and fill it with the class's textureCoords vector
	glGenBuffers(1, &texBufQuadID);
	glBindBuffer(GL_ARRAY_BUFFER, texBufQuadID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(aTexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//create the element buffer that indicates the order of the vertices to draw the triangles forming the quad.
	glGenBuffers(1, &quadIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

QuadTextured::~QuadTextured()
{
}

void QuadTextured::draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 lightPV) {
	program->bind();
	M->pushMatrix();
	glBindVertexArray(quadVAO);

	// The texture should already be bound at this point, but we'll call this for good measure.
	glActiveTexture(GL_TEXTURE1); // use texture unit 1
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glUniform1i(program->getUniform("depthMap"), 1); // texture unit ID for depth map

	glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);


	// Bind position buffer
	int h_pos = program->getAttribute("aPos");
	glEnableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufQuadID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	// Bind texcoords buffer
	int h_tex = program->getAttribute("aTex");
	glEnableVertexAttribArray(h_tex);
	glBindBuffer(GL_ARRAY_BUFFER, texBufQuadID);
	glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	// Draw
	int count = 4; // number of indices to be rendered
	glDrawArrays(GL_TRIANGLE_STRIP, 0, count);

	// Disable and unbind
    glDisableVertexAttribArray(h_tex);
	glDisableVertexAttribArray(h_pos);

	glBindVertexArray(0);
	M->popMatrix();
	program->unbind();
	GLSL::checkError(GET_FILE_LINE);
}
