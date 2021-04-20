#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 BaseColor;
in vec3 fragNormal;
in vec3 fragPos;
uniform sampler2D texture_diffuse1;

uniform vec3 Light1Direction = normalize(vec3(-1, 0, 0));
uniform vec3 Light1Color = vec3(1.0, 1.0, 1.0);
uniform vec3 viewPos = vec3(0.0, 0.0, 0.0);

void main()
{
    //FragColor = texture(texture_diffuse1, TexCoords);
   
    // ambient 
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * vec3(BaseColor.x, BaseColor.y, BaseColor.z);
  	
    // diffuse for Light1
    float diffuseStrength = 0.75;
    vec3 norm = normalize(fragNormal);
    float diff = max(dot(norm, Light1Direction), 0.0);
    vec3 diffuse = diff * Light1Color * diffuseStrength;
    
    // specular for Light1
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-Light1Direction, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * Light1Color;  
        
    vec3 result = (ambient + diffuse + specular) * vec3(BaseColor.x, BaseColor.y, BaseColor.z);
    FragColor = vec4(result, 1.0);
}
