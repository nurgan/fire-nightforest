#version 330 core 

in vec3 fragNormal;
in vec4 worldPos;

uniform vec3 specularMatColor;
uniform float shininess;
uniform vec3 diffuseMatColor;
uniform vec3 ambientMatColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewVec;
uniform vec3 eyePos;

out vec4 color;

void main()
{

   // "basic" phong lighting
   vec3 normal = normalize(fragNormal);
   vec3 lightDir = normalize(lightPos.xyz - worldPos.xyz);
   vec3 viewDir = normalize(viewVec);
   vec3 reflectDir = normalize(reflect(lightDir, normal)); 

   vec3 ambient = ambientMatColor * lightColor;
   vec3 diffuse = vec3(0.0);
   vec3 specular = vec3(0.0);

   float diffuseFactor = max(dot(lightDir, normal), 0);
   if(diffuseFactor > 0.0){
      diffuse = diffuseFactor * diffuseMatColor * lightColor;
      specular = pow(max(dot(viewDir, reflectDir), 0), shininess) * specularMatColor * lightColor;
   }

   // calculate distance falloff
   float dist =  length(worldPos.xyz - lightPos); 
   float falloff = 1.0 / (1.0 + 0.05*dist + 0.01*dist*dist);
   vec3 actualColor = ambient + diffuse + specular;
   actualColor *= falloff;

   // calculate greyscale value for color
   float lumiosity = 0.21 * actualColor.r + 0.72 * actualColor.g + 0.07 * actualColor.b;
   
   // color as mix of greyscale and actual color dependend on distance falloff
   color = vec4(falloff * actualColor + (1.0 - falloff) * vec3(lumiosity), 1.0);   


   // spotlight aka flash light
   float cutOff = 0.998;
   float outerCutOff = 0.955;
   
   // calculate spotlight intensity
   vec3 vertDir = worldPos.xyz - eyePos;
   float spotCos = dot(normalize(viewDir), normalize(vertDir)); 
   float epsilon   = cutOff - outerCutOff;
   float intensity = clamp((spotCos - outerCutOff) / epsilon, 0.0, 1.0);  

   // if in spotlight
   if(intensity > 0.0){

      // calculate spotlight lighting
      float dist =  length(worldPos.xyz - eyePos);
      float falloff = 1.0 / (1.0 + 0.05*dist + 0.005*dist*dist);
      vec3 reflectDir = normalize(reflect(-viewDir, normal));
      diffuseFactor = max(dot(-viewDir, normal), 0);
      if(diffuseFactor > 0.0){
         diffuse = diffuseFactor * diffuseMatColor * lightColor;
         specular = pow(max(dot(-viewDir, reflectDir), 0), shininess) * specularMatColor * lightColor;
      }
      // add spotlight color
      color += vec4(falloff * intensity * (diffuse + specular), 1.0);
   }

   


}
