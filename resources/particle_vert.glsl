#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec4 Pcolor;
layout(location = 2) in float lifePercent;

uniform mat4 MVP;

out vec4 partCol;
out float life;

void main() { 

	gl_Position = MVP * vec4(vertPos.xyz, 1.0);
	partCol = Pcolor;
   life = lifePercent;
}
