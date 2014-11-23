#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform int texID;


const float ambient = .3;
void main() {
   vec3 color;
   if (texID == -1)
   	 color = Color;
   else if (texID == 0)
     color = texture(tex0, texcoord).rgb;
   else if (texID == 1)
     color = texture(tex1, texcoord).rgb;  
   else{
   	 outColor = vec4(0,1,0,1);
   	 return; //This was an error, stop lighting!
   	}
   vec3 diffuseC = color*max(dot(-lightDir,normal),0.0);
   vec3 ambC = color*ambient;
   vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
   vec3 reflectDir = reflect(viewDir,normal);
   float spec = max(dot(reflectDir,lightDir),0.0);
   if (dot(-lightDir,normal) <= 0.0)spec = 0;
   vec3 specC = .8*vec3(1.0,1.0,1.0)*pow(spec,4);
   vec3 oColor = ambC+diffuseC+specC;

   // Cel Shading
   float mag = 0.3*min(color.r,1) + 0.6*min(color.g, 1) + 0.1 * min(color.b, 1);
   if(mag < 0.3) {
      outColor = (0.3/mag)*outColor;
   }
   else if(mag < 0.6) {
      outColor = (0.6/mag)*outColor;
   }
   else if(mag < 0.9) {
      outColor = (0.9/mag)*outColor;
   }
   else {
      //outColor = vec4(1, 1, 1, 1);
      outColor = (0.9/mag)*outColor;
   }

   if(dot(normal,viewDir) < 0.32) outColor = vec4(0,0,0,1);
   if(dot(normal,viewDir) > 0.94) outColor = vec4(1,1,1,1);


   //outColor = vec4(oColor,1);   // commented out from original by NoW @ 11/22/14 @ 5:10 PM
}