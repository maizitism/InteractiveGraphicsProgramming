#version 330 core

layout (location=0) in vec3 pos;

uniform mat4 mvp, mv;

out float vDepth;

void main(){
    gl_Position = mvp * vec4(pos, 1); // position on screen
    vec4 viewPos = mv * vec4(pos, 1); // location in camera space
    vDepth = -viewPos.z; // distance from camera (positive)
}