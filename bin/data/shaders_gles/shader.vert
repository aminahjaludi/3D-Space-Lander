#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float size;
layout(location = 2) in vec4 color;
layout(location = 3) in float lifeRatio; // Value between 0.0 (birth) and 1.0 (death)

out vec4 vColor;
out float vLifeRatio;

uniform mat4 modelViewProjectionMatrix;

void main() {
    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
    gl_PointSize = size;
    vColor = color;
    vLifeRatio = lifeRatio;
}
