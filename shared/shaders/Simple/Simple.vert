#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;

out vec4 Color;

uniform vec3 u_Scale;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    // setup position
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position * u_Scale, 1.0);

    // setup color
    Color = a_Color;

    // setup point size
    gl_PointSize = 10.f;
}