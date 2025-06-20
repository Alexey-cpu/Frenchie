#version 330 core
// vertex attributes
layout (location = 0) in vec3 aPos;

//uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // setup model position
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}