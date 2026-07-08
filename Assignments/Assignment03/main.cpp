#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cyTriMesh.h"
#include "cyMatrix.h"

#include <cyGL.h>
#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <vector>
#include <unordered_map>


// --- GLOBALS --- //
int width = 800;
int height = 600;
const float PI = 3.14159f;
cy::GLSLProgram prog;

struct Camera {
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = -45.0f;
    float distance = 45.0f;
    float maxDist = 100.0f;
    float minDist = 20.0f;
    float sensitivity = 0.2f;
};
struct Mouse{
    int button = 0;
    int previousX = 0;
    int previousY = 0;
};
struct Scene{
    unsigned int vaoID[1];
    unsigned int vboID[1]; 
    unsigned int nMesh;
    cy::Matrix4f model;
    cy::Matrix4f proj;
    cy::Vec3f bbMax;
    cy::Vec3f bbMin;
    float radius;
};

Camera cam;
Mouse mouse;
Scene scene;

cy::Vec4f viewPos;

float deg2rad(float deg){
    return deg * PI/180;
}

uint64_t pack(unsigned int pIDX, unsigned int nIDX){
    return (uint64_t(pIDX) << 32) | uint32_t(nIDX);
}

void updateMatrices(){
    cy::Matrix4f rotXYZ = cy::Matrix4f::RotationXYZ(deg2rad(cam.roll), deg2rad(cam.yaw), deg2rad(cam.pitch));
    cy::Matrix4f trans = cy::Matrix4f::Translation(cy::Vec3f(0, 0, -cam.distance));
    cy::Matrix4f mv;
    cy::Matrix4f mvp;
    
    mv = trans * rotXYZ * scene.model;
    mvp = scene.proj * mv;
    prog["mv"] = mv;
    prog["mvp"] = mvp;
    prog["nearRange"] = cam.distance - scene.radius;
    prog["farRange"] = cam.distance + scene.radius;
}

void computeStatics(cy::TriMesh& mesh){

    float fovRadians = 60 * 3.14159/180;
    float aspect = 800.0f/600.0f;
    float zNear = 0.1f;
    float zFar = 5000.0f;
    mesh.ComputeBoundingBox();
    scene.bbMax = mesh.GetBoundMax();
    scene.bbMin = mesh.GetBoundMin();
    scene.radius = (scene.bbMax-scene.bbMin).Length() * 0.5;

    cy::Vec3f center = (scene.bbMin + scene.bbMax) * 0.5f;
    
    scene.model = cy::Matrix4f::Translation(-center);
    scene.proj = cy::Matrix4f::Perspective(fovRadians, aspect, zNear, zFar);
}

// --- OpenGL things --- //
void myDisplay() {
    // clear the viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //opengl draw calls
    glDrawArrays(GL_POINTS, 0, scene.nMesh);

    // swap buffers
    glutSwapBuffers();
}

// -- Registered callbacks -- //
void myMouse(int button, int state, int x, int y) {
    mouse.button = button;
    mouse.previousX = x;
    mouse.previousY = y;
}

void myMouseMotion(int x, int y) {
    switch(mouse.button){
        case 0: // LMB
            cam.yaw += (x-mouse.previousX) * cam.sensitivity;
            cam.roll += (y-mouse.previousY) * cam.sensitivity;
        break;
        case 1: // MMB
            cam.pitch += (x - mouse.previousX) * cam.sensitivity;
        break;
        case 2: // RMB
            cam.distance = std::clamp(cam.distance + (y - mouse.previousY) * cam.sensitivity, cam.minDist, cam.maxDist);
        break;
        default:
            printf("What yall doin here?");
            break;
    }
    // LMB drag moves rotates on X and Z (ID 0)
    // RMB drag moves camera distance (ID 2)
    // MMB drag rotates on Z (ID 1) 
    //printf("%f, %f, %f, %f\n", roll, pitch, yaw, distance);
    updateMatrices();
    mouse.previousX = x;
    mouse.previousY = y;
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
    scene.nMesh = mesh.NV();
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

    glGenVertexArrays(1, &scene.vaoID[0]); // first, make VAO
    glBindVertexArray(scene.vaoID[0]); // bind it, set it as operational

    // EBO impl
    struct Vertex{
        cy::Vec3f position;
        cy::Vec3f normal;
    };
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<uint64_t, unsigned int> seen;

    for(unsigned int i = 0; i < mesh.NF(); i++ ){
        cy::TriMesh::TriFace posFace = mesh.F(i);
        cy::TriMesh::TriFace normalsFace = mesh.FN(i);
        //iterate over every vertex in face
        for(unsigned int j = 0; j<=2; j++){
            unsigned int positionIndex = posFace.v[j];
            unsigned int normalIndex = normalsFace.v[j];
            uint64_t key = pack(positionIndex, normalIndex);
            if(auto search = seen.find(key); search != seen.end()){
                indices.emplace_back(search->second);
            }
            else{
                Vertex v;
                v.position = mesh.V(positionIndex);
                v.normal = mesh.VN(normalIndex);
                size_t slot = vertices.size();
                vertices.emplace_back(v);
                indices.emplace_back(slot);
                seen[key] = slot;
            }
        }
    }

    // VBO impl 

    glGenBuffers(1, &scene.vboID[0]);
    glBindBuffer(GL_ARRAY_BUFFER, scene.vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f)*mesh.NV(), &mesh.V(0), GL_STATIC_DRAW);


    // for each face i and corner j, read the position and normals
    // look at a "seen" structure
        // if seen push stored index into indices,
        // if not, build the vertex, push it to the end and put it into seen
    
    // faces in cyTriMesh are returned with cy::triface face = mesh.F()
    // cy::triface has 3 vertex indices - face.v[]
    // normals for face indices can be gotten by cy::TriFace faceNormal = mesh.FN()
    // they again have vertex indices, faceNormal.v[]

    // create data new structure: struct: Vertex, contains cy::vec3f pos, and cy::vec3f normal
    // make a vector out of them std::vector <Vertex> vertices - the new VBO
    // and keep their ordering in a std::vector<unsigned int> indices - the EBO
   

    // tell about everything to vertex shader
    GLuint pos = glGetAttribLocation(prog.GetID(), "pos"); // find position of pos in variable in vertex shader
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    computeStatics(mesh);
    updateMatrices();
   
    prog.Bind();
    glutMainLoop();
    return 0;
}