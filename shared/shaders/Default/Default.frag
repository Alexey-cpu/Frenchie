#version 330 core

//outputs
out vec4 fragColor;

//uniforms
uniform vec4 u_Color;

void main()
{
    // setup vertex color
    fragColor = u_Color;
}