#version 330 core
uniform vec4 u_Color;

void main()
{
    // setup vertex color
    gl_FragColor = u_Color;
    //gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}