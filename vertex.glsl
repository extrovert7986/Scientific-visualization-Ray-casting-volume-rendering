#version 450 core

out vec3 position;
out vec3 texCoord;
out mat3 NormalMatrix;
out vec3 direc;
out vec3 wVec;

layout (location = 0)in vec3 vertexPosition;
layout (location = 1)in vec3 vertexTexCoord;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main(){
    position = vec3(modelViewMatrix*vec4(vertexPosition,1.0));
    texCoord = vertexTexCoord;
    NormalMatrix = mat3(modelViewMatrix[0].xyz,modelViewMatrix[1].xyz,modelViewMatrix[2].xyz);
    direc = vec3(inverse(modelViewMatrix) * vec4(0.0,0.0,-1.0,0.0));
    wVec = vec3(modelViewMatrix[2]);
    gl_Position = projectionMatrix*vec4(position,1.0);

}