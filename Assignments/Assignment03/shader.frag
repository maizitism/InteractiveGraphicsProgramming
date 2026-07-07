#version 330 core

in float vDepth;

layout(location=0) out vec4 color;

uniform float nearRange;
uniform float farRange;


void main(){
    float t = clamp((vDepth-nearRange) / (farRange - nearRange), 0, 1);
    color = vec4(vec3(1.0 - t), 1);
}