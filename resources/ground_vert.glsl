#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 M;
uniform mat4 MVP;
uniform mat4 transInvM;

out vec2 vTexCoord;
out vec3 fragNor;
out vec4 worldPos;

void main() {

   vec3 lightDir = vec3(1, 1, 1);
   vec4 vPosition;

   gl_Position = MVP * vec4(vertPos.xyz, 1.0);
   fragNor = normalize(transInvM * vec4(vertNor, 0.0)).xyz;
   worldPos = M * vec4(vertPos, 1.0);
   vTexCoord = vertTex;
}
