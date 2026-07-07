#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cyTriMesh.h"
#include "cyMatrix.h"

#include <cyGL.h>
#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>


// --- GLOBALS --- //
int width = 800;
int height = 600;

unsigned int vaoID[1];
unsigned int vboID[1]; 
unsigned int nMesh = 0;
cy::GLSLProgram prog;
cy::Matrix4f model;
cy::Matrix4f proj;
cy::Matrix4f rotXYZ;
cy::Matrix4f trans;
cy::Matrix4f mvp;

cy::Vec3f bbMax;
cy::Vec3f bbMin;

cy::Matrix4f mv;
cy::Vec4f viewPos;
float radius;
float camDist;


int mouseDownState = 0;
int mousePrevX = 0;
int mousePrevY = 0;
float roll = 0.0f; // accumulate rotation around X
float pitch = 0.0f; // accumulate rotation around Y
float yaw = 0.0f; // accumulate rotation around Z
float distance = 45.0f;
int maxCameraDistance = 100;
int minCameraDistance = 20;
float sensitivity = 0.2f;


// --- OpenGL things --- //
void myDisplay() {
    // clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //opengl draw calls
    glDrawArrays(GL_POINTS, 0, nMesh);

    // swap buffers
    glutSwapBuffers();
}

// -- Registered callbacks -- //
void myMouse(int button, int state, int x, int y) {
    mouseDownState = button;
    mousePrevX = x;
    mousePrevY = y;
}

void myMouseMotion(int x, int y) {
    switch(mouseDownState){
        case 0: // LMB
            yaw += (x-mousePrevX) * sensitivity;
            roll += (y-mousePrevY) * sensitivity;
        break;
        case 1: // MMB
            pitch += (x - mousePrevX) * sensitivity;
        break;
        case 2: // RMB
            distance += (y - mousePrevY) * sensitivity;
        break;
        default:
            printf("What yall doin here?");
            break;
    }
    // LMB drag moves rotates on X and Z (ID 0)
    // RMB drag moves camera distance (ID 2)
    // MMB drag rotates on Z (ID 1) 
    printf("%f, %f, %f, %f\n", roll, pitch, yaw, distance);
    camDist = std::clamp(distance, (float)minCameraDistance, (float)maxCameraDistance);
    trans = cy::Matrix4f::Translation(cy::Vec3f(0, 0, -camDist));
    rotXYZ = cy::Matrix4f::RotationXYZ(roll * 3.14159/180, yaw * 3.14159/180, pitch * 3.14159/180);
    
    mv = trans * rotXYZ * model;
    mvp = proj * mv;
    prog["mv"] = mv;
    prog["mvp"] = mvp;
    radius = (bbMax-bbMin).Length() * 0.5;
    prog["nearRange"] = camDist - radius;
    prog["farRange"] = camDist + radius;
    mousePrevX = x;
    mousePrevY = y;
    glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y) {
    
    printf("Key pressed: %d\n", key);
	if (key == 27){
        glutLeaveMainLoop();
    }
	// modifiers can be checked with
    printf("%d\n", glutGetModifiers());
}

void myModifiers(int key, int x, int y) {
    if (key == GLUT_KEY_F6){
        printf("Shaders recompiled. \n");
        prog.BuildFiles("shader.vert", "shader.frag");
    } 
}

// void myIdle() {

//     roll += 0.2;

//     camDist = std::clamp(distance, (float)minCameraDistance, (float)maxCameraDistance);
//     trans = cy::Matrix4f::Translation(cy::Vec3f(0, 0, -camDist));
//     rotXYZ = cy::Matrix4f::RotationXYZ(roll * 3.14159/180, yaw * 3.14159/180, pitch * 3.14159/180);
//     mv = trans * rotXYZ * model;
//     mvp = proj * mv;
//     prog["mv"] = mv;
//     prog["mvp"] = mvp;
//     radius = (bbMax-bbMin).Length() * 0.5;
//     prog["nearRange"] = camDist - radius;
//     prog["farRange"] = camDist + radius;
//     glutPostRedisplay();
// }



void initGlew(){
    GLenum err = glewInit();
    if(err != GLEW_OK){
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
}

cy::TriMesh loadMesh(char* filename){
    cy::TriMesh mesh;
    if (!mesh.LoadFromFileObj(filename)) {
        fprintf(stderr, "Error: Could not load mesh %s\n", filename);
    }
    return mesh;
}

int main(int argc, char** argv)
{

    // Glut init + create window
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitContextFlags(GLUT_DEBUG);
    glutCreateWindow("Window title!!");

    glutMouseFunc(myMouse);
    glutMotionFunc(myMouseMotion);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(myModifiers);
    //glutIdleFunc(myIdle);

    initGlew();
    CY_GL_REGISTER_DEBUG_CALLBACK;
    glutDisplayFunc(myDisplay);
    glClearColor(0, 0, 0, 0);

    //initialise glew
    
    // load the mesh
    if(argc < 2){
        fprintf(stderr, "Error: No File Given as Argument.");
        return -1;
    }
    cy::TriMesh mesh = loadMesh(argv[1]);
    nMesh = mesh.NV();
    // initialise shaders   
    unsigned int success = prog.BuildFiles("shader.vert", "shader.frag");
    if (!success){
        fprintf(stderr, "Error: Failed to compile or link shaders.");
        return -1;
    }
    // load vertex data
    // VBO - "box" of raw data
    // VAO - "shipping label" - records how to interpret VBO
    // VAO impl

    glGenVertexArrays(1, &vaoID[0]); // first, make VA
    glBindVertexArray(vaoID[0]); // bind it, set it as operational

    // VBO impl 

    glGenBuffers(1, &vboID[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f)*mesh.NV(), &mesh.V(0), GL_STATIC_DRAW);

    // tell about everything to vertex shader
    GLuint pos = glGetAttribLocation(prog.GetID(), "pos"); // find position of pos in variable in vertex shader
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);


    // build MVP matrix - Model, View, Projection
    float fovRadians = 60 * 3.14159/180;
    float aspect = 800.0f/600.0f;
    float zNear = 0.1f;
    float zFar = 5000.0f;

    float angleRadians = -45 * 3.14159/180;

    mesh.ComputeBoundingBox();
    bbMax = mesh.GetBoundMax();
    bbMin = mesh.GetBoundMin();

    cy::Vec3f center = (bbMin + bbMax) * 0.5f;

    model = cy::Matrix4f::Translation(-center);
    proj = cy::Matrix4f::Perspective(fovRadians, aspect, zNear, zFar);
    rotXYZ = cy::Matrix4f::RotationXYZ(angleRadians, 0, 0);
    trans = cy::Matrix4f::Translation(cy::Vec3f(0, 0, -45));


    mv = trans * rotXYZ * model;
    mvp = proj * mv;
    prog["mv"] = mv;
    prog["mvp"] = mvp;
    // compute the nearRange and farRange bounding box diagonal for coloring
    radius = (bbMax-bbMin).Length() * 0.5;
    prog["nearRange"] = distance - radius;
    prog["farRange"] = distance + radius;
    
    prog.Bind();
    glutMainLoop();
    return 0;
}