#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 BaseColor;
out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 aBaseColor;
uniform mat4 viewProjMtx;

void main()
{
    TexCoords = aTexCoords;
    BaseColor = aBaseColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragNormal = mat3(transpose(inverse(model))) * aNormal; 
    fragPos = vec3(model * vec4(aPos, 1.0));
}
