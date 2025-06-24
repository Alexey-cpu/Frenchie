#version 330 core

// inputs
in vec3 Normal;
in vec2 UV;
in vec4 Color;

void main()
{
    // setup vertex color
    gl_FragColor = Color;
}