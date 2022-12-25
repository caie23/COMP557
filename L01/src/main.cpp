#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Motion.h"
#include "DAGNode.h"

using namespace std;

GLFWwindow *window; // Main application window
string RES_DIR = ""; // Where data files live
shared_ptr<Program> prog;
shared_ptr<Program> prog2; // for drawing with colours
shared_ptr<Shape> shape;
shared_ptr<Motion> motion = make_shared<Motion>();

//GLuint VAO;
//GLuint VBO; // position buffer for drawing a line loop
GLuint aPosLocation = 0; // location set in col_vert.glsl (or can be queried)
//const GLuint NumVertices = 6;
//GLfloat vertices[NumVertices][6] = {
//                    { 0, 0, 0, 1, 0, 0 }, // red x-axis
//                    { 1, 0, 0, 1, 0, 0 },
//                    { 0, 0, 0, 0, 1, 0 }, // green y-axis
//                    { 0, 1, 0, 0, 1, 0 },
//                    { 0, 0, 0, 0, 0, 1 }, // blue z-axis
//                    { 0, 0, 1, 0, 0, 1 } };

static void error_callback(int error, const char *description)
{
    cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

static void init()
{
    GLSL::checkVersion();

    // Check how many texture units are supported in the vertex shader
    int tmp;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tmp);
    cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = " << tmp << endl;
    // Check how many uniforms are supported in the vertex shader
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &tmp);
    cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS = " << tmp << endl;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
    cout << "GL_MAX_VERTEX_ATTRIBS = " << tmp << endl;

    // Set background color.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);

    // Initialize mesh.
    shape = make_shared<Shape>();
    shape->loadMesh(RES_DIR + "cube.obj");
    shape->init();
    
    // Initialize the GLSL programs.
    prog = make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames(RES_DIR + "nor_vert.glsl", RES_DIR + "nor_frag.glsl");
    prog->init();
    prog->addUniform("P");
    prog->addUniform("MV");
    prog->addAttribute("aPos");
    prog->addAttribute("aNor");
    prog->setVerbose(false);
    
    prog2 = make_shared<Program>();
    prog2->setVerbose(true);
    prog2->setShaderNames(RES_DIR + "col_vert.glsl", RES_DIR + "col_frag.glsl");
    prog2->init();
    prog2->addUniform("P");
    prog2->addUniform("MV");
    prog2->addUniform("col");
    prog2->addAttribute("aPos");
    prog2->setVerbose(false);
    
    // If there were any OpenGL errors, this will print something.
    // You can intersperse this line in your code to find the exact location
    // of your OpenGL error.
    GLSL::checkError(GET_FILE_LINE);
}


class Axis{
    float ox, oy, oz, scale;
    GLuint VAO;
    GLuint VBO; // position buffer for drawing a line loop
    GLfloat vertices[6][6] = {
                        { 0, 0, 0, 1, 0, 0 }, // red x-axis
                        { 1, 0, 0, 1, 0, 0 },
                        { 0, 0, 0, 0, 1, 0 }, // green y-axis
                        { 0, 1, 0, 0, 1, 0 },
                        { 0, 0, 0, 0, 0, 1 }, // blue z-axis
                        { 0, 0, 1, 0, 0, 1 } };
    
public:
    Axis(float x, float y, float z, float s) {
        ox = x;
        oy = y;
        oz = z;
        scale = s;
        vertices[0][0] = ox;
        vertices[0][1] = oy;
        vertices[0][2] = oz;
        vertices[1][0] = ox+1*scale;
        vertices[1][1] = oy;
        vertices[1][2] = oz;
        vertices[2][0] = ox;
        vertices[2][1] = oy;
        vertices[2][2] = oz;
        vertices[3][0] = ox;
        vertices[3][1] = oy+1*scale;
        vertices[3][2] = oz;
        vertices[4][0] = ox;
        vertices[4][1] = oy;
        vertices[4][2] = oz;
        vertices[5][0] = ox;
        vertices[5][1] = oy;
        vertices[5][2] = oz+1*scale;
    }
    template <typename T>
    int draw(T P, T MV) {
        // Create buffers
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Specify the position in the VAO and enable it
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Specify the colour in the VAO and enable it
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Un bind the buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Draw axis
        prog2->bind();
        MV->pushMatrix();
        glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
        glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
        glBindVertexArray(VAO);
        glUniform3f(prog2->getUniform("col"), 1, 0, 0);  // red x-axis
        glDrawArrays(GL_LINE_LOOP, 0, 2);
        glUniform3f(prog2->getUniform("col"), 0, 1, 0);  // green y-axis
        glDrawArrays(GL_LINE_LOOP, 2, 2);
        glUniform3f(prog2->getUniform("col"), 0, 0, 1);  // blue z-axis
        glDrawArrays(GL_LINE_LOOP, 4, 2);
        MV->popMatrix();
        prog2->unbind();

        return 1;
    }
};

template <typename T>
void drawNodeFrames(DAGNode* root, glm::fvec3 acc_offset, T P, T MV, int frameCounter) {
    std::vector <DAGNode*> children = root->children;
    for (DAGNode* child : children) {
        // accumulate the offset
        float accx = acc_offset.x + child->offset.x;
        float accy = acc_offset.y + child->offset.y;
        float accz = acc_offset.z + child->offset.z;
        // channel j of frame i can be accessed with data[i*numChannels+j]
        float* data = motion->data;
        int channelDataStartIndex = child->channelDataStartIndex;
        int numchannels = motion->numChannels;
        int channelIdx = frameCounter * numchannels + channelDataStartIndex;
        float Xrotation = data[channelIdx];
        float Yrotation = data[channelIdx+1];
        float Zrotation = data[channelIdx+2];
        MV->rotate(Xrotation, glm::vec3(1, 0, 0));
        MV->rotate(Yrotation, glm::vec3(0, 1, 0));
        MV->rotate(Zrotation, glm::vec3(0, 0, 1));
        Axis currootframe(accx, accy, accz, 0.1);
        currootframe.draw(P, MV);
        
        drawNodeFrames(child, glm::vec3(accx, accy, accz), P, MV, frameCounter);
    }
}

static void render(int frameCounter)
{
    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = width/(float)height;
    if (isnan(aspect))
    {
        aspect = 0;
    }
    glViewport(0, 0, width, height);

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Create matrix stacks.
    auto P = make_shared<MatrixStack>();
    auto MV = make_shared<MatrixStack>();
    // Apply projection.
    P->pushMatrix();
    P->multMatrix(glm::perspective((float)(45.0*M_PI/180.0), aspect, 0.01f, 100.0f));
    // Apply camera transform.
    MV->pushMatrix();
    MV->translate(glm::vec3(0, 0, -3));
    MV->rotate(0.3, glm::vec3(1, 1, 1));

    // draw the world frame
    Axis worldframe(0, 0, 0, 1);
    worldframe.draw(P, MV);
    
    // Draw a coordinate frame for the root frame
    DAGNode* rootnode = motion->root; // Hips
    float* data = motion->data;
    float rootx = rootnode->offset.x + data[0];  // Xposition
    float rooty = rootnode->offset.y + data[1];  // Yposition
    float rootz = rootnode->offset.z + data[2];  // Zposition
    Axis rootframe(rootx, rooty, rootz, 0.1);
    float Xrotation = data[3];
    float Yrotation = data[4];
    float Zrotation = data[5];
    MV->rotate(Xrotation, glm::vec3(1, 0, 0));
    MV->rotate(Yrotation, glm::vec3(0, 1, 0));
    MV->rotate(Zrotation, glm::vec3(0, 0, 1));
    rootframe.draw(P, MV);
    
    // Draw cube
    prog->bind();
    MV->pushMatrix();
    MV->scale(0.01, 0.01, 0.01);
    MV->translate(glm::vec3(rootx, rooty, rootz));
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
    shape->draw(prog);
    MV->popMatrix();
    prog->unbind();
    
    // Draw coordinate frames for all nodes
    drawNodeFrames(rootnode, glm::vec3(rootx, rooty, rootz), P, MV, frameCounter);

    // Draw skeleton.
    // channel j of frame i can be accessed with data[i*numChannels+j]
    int numchannels = motion->numChannels;
    float* framedata = data + frameCounter * numchannels;
    rootnode->draw(prog, P, MV, glm::vec3(rootx, rooty, rootz), shape, framedata);

    // Pop matrix stacks.
    MV->popMatrix();
    P->popMatrix();

    GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
    if(argc < 2) {
        cout << "Please specify the resource directory." << endl;
        return 0;
    }
    RES_DIR = argv[1] + string("/");

    // Set error callback.
    glfwSetErrorCallback(error_callback);
    // Initialize the library.
    if(!glfwInit()) {
        return -1;
    }
    
    // https://en.wikipedia.org/wiki/OpenGL
    // hint to use OpenGL 4.1 on all paltforms
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(640, 480, "Claire Feng 260899684", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    // Make the window's context current.
    glfwMakeContextCurrent(window);
    // Initialize GLEW.
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }
    glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    // Set vsync.
    glfwSwapInterval(1);
    // Set keyboard callback.
    glfwSetKeyCallback(window, key_callback);
    // Initialize scene.
    init();
    
    // load bvh file
    motion->loadBVH(RES_DIR + "0019_AdvanceBollywoodDance001.bvh");
    
    // apply a uniform scale of 1/100
    // scale the offset of every node
    DAGNode* rootnode = motion->root;
    rootnode->scaleOffset(0.01); // recursive call
    
    // scale the per-frame data for all channels
    int numframes = motion->numFrames;
    int numchannels = motion->numChannels;
    float* data = motion->data;
    for (int i = 0; i < numframes; i++) {
        for (int j = 0; j < numchannels; j++) {
            // channel j of frame i can be accessed with data[i*numChannels+j]
            data[i*numchannels+j] *= 0.01;
        }
    }
    
    int frameCounter = 0; // look up the appropriate frame data
    // Loop until the user closes the window.
    while(!glfwWindowShouldClose(window)) {
        // Render scene.
        render(frameCounter);
        frameCounter ++;
        if (frameCounter == numframes) {
            frameCounter = 0;
        }
        // Swap front and back buffers.
        glfwSwapBuffers(window);
        // Poll for and process events.
        glfwPollEvents();
    }
    // Quit program.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
