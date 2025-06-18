#version 330 core

// vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// output variables
out vec3 ourColor;
out vec2 TexCoord;

//uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // setup model position
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // do smth. with texture color
    ourColor = aColor;

    // do smth. with texture coordinates
    TexCoord = aTexCoord;
}