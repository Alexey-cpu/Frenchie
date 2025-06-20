#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;

// texture samplers
uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos; 

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3  ambient         = ambientStrength * lightColor;
  	
    // diffuse 
    vec3  norm     = normalize(Normal);
    vec3  lightDir = normalize(vec3(0.f, 10.f, 0.f));//normalize(lightPos - FragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

	// multiply texture color by a light color
	FragColor = texture(texture1, TexCoord) * vec4(ambient + diffuse + specular, 1.f);
}