#version 330 core

// vertex attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;
layout (location = 3) in vec4 a_Color;

// outputs
out vec3 Normal;
out vec2 UV;
out vec4 Color;

// uniforms
uniform vec3 u_ViewportScale;
uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main()
{
    // setup position
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(a_Position * u_ViewportScale, 1.0);

    // setup outputs
    Normal = a_Normal;
    UV     = a_UV;
    Color  = a_Color;
}