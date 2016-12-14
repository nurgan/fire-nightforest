#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec3 vertTex;

uniform mat4 M;
uniform mat4 MVP;
uniform mat4 transInvM;

out vec3 fragNormal;
out vec4 worldPos;
out vec4 worldPosV;

void main()
{

   gl_Position = MVP * vertPos;
   fragNormal = normalize(transInvM * vec4(vertNor, 0.0)).xyz;
   worldPos = M * vertPos;

}
