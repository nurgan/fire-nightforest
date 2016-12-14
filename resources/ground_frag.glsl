#version 330 core

in vec2 vTexCoord;
in vec3 fragNor;
in vec4 worldPos;

uniform sampler2D Texture0;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewVec;
uniform vec3 eyePos;

out vec4 color;

void main() {
      
   vec3 texColor = texture(Texture0, vTexCoord).rgb;

   // "basic" phong lighting including distance falloff
   vec3 normal = normalize(fragNor);
   vec3 lightDir = normalize(lightPos - worldPos.xyz);
   float diffuseFactor = 2.0 * max(dot(lightDir, normal), 0);
   float dist =  length(worldPos.xyz - lightPos );
   float falloff = 1.0 / (1.0 + 0.05*dist + 0.005*dist*dist);
 
   // calculate actual color of texture dependent on diffuse lighting
   vec3 col = falloff * diffuseFactor * texColor;
    
   // calculate greyscale value for color
   float lumiosity = 0.21 * col.r + 0.72 * col.g + 0.07 * col.b;
    
   // color as mix of greyscale and actual color dependend on distance falloff
   color = vec4(falloff * col + (1.0 - falloff) * vec3(lumiosity), 1.0);
	

   // spotlight aka flash light
   vec3 viewDir = normalize(viewVec);
   float cutOff = 0.998;
   float outerCutOff = 0.955;
   vec3 vertDir = worldPos.xyz - eyePos;
   float spotCos = dot(normalize(viewDir), normalize(vertDir)); 
   float epsilon   = cutOff - outerCutOff;
   float intensity = clamp((spotCos - outerCutOff) / epsilon, 0.0, 1.0);  

   // if in spotlight
   if(intensity > 0.0){

      float dist =  length(worldPos.xyz - eyePos);
      float falloffSpot = 1.0 / (1.0 + 0.05*dist + 0.005*dist*dist);

      vec3 reflectDir = normalize(reflect(-viewDir, normal));
      diffuseFactor = max(dot(-viewDir, normal), 0);

      // add texture color for spotlight
      color += vec4(falloffSpot * intensity * diffuseFactor * texColor, 0.0);
   }


}

