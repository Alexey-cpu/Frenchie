#version 330 core

// vertex attributes
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

// output variables
out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

//uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // light
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal  = mat3(transpose(inverse(model))) * aNormal;  

    mat3(transpose(inverse(model)));

    // setup model position
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // do smth. with texture coordinates
    TexCoord = aTexCoord;
}