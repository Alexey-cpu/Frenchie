#version 330 core
uniform vec4 u_Color;

void main()
{
    // setup vertex color
    gl_FragColor = u_Color;
}