#version 330 core

in vec4 partCol;
in float life;

uniform sampler2D alphaTexture;

out vec4 color;

void main()
{

   // Scale texture coordinates to access correct fire alpha texture in texture atlas.
   // "Strange" scaling because first texture is in upper left corner, 
   // and texture coordinates have their origin in lower left corner.
   vec2 pointCoord = gl_PointCoord * 0.25;
   float scale = floor(life * 16.0);
   if(scale  < 4.0){
      pointCoord.y = pointCoord.y + 0.75;
   } else if(scale < 8.0){
      pointCoord.y = pointCoord.y + 0.5;
   } else if(scale < 12.0){
      pointCoord.y = pointCoord.y + 0.25;
   }
   if(mod(scale, 4) == 1){
      pointCoord.x = pointCoord.x + 0.25;
   } else if(mod(scale, 4) == 2){
      pointCoord.x = pointCoord.x + 0.5;
   } else if(mod(scale, 4) == 3){
      pointCoord.x = pointCoord.x + 0.75;
   }

	float alpha = texture(alphaTexture, pointCoord).r;
	color = vec4(partCol.rgb, partCol.a*alpha);
}
