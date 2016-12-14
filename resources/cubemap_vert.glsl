#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec3 vertTex;
uniform mat4 MVP;

out vec3 vTexCoord;

void main() {

  gl_Position = MVP * vec4(vertPos.xyz, 1.0);

  vTexCoord = vertPos;  

}
