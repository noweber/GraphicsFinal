#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;
in vec2 texcoord;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;

uniform int texID;
uniform int drawOutline;
uniform int UIRender;
uniform vec2 texOffset;


const float ambient = .3;
void main() {
  vec3 color;

  if (texID == -1)
           color = Color;
  else if (texID == 0)
    color = texture(tex0, texcoord).rgb;
  else if (texID == 1)
    color = texture(tex1, texcoord).rgb;
  else if (texID == 2)
    color = texture(tex2, texcoord).rgb;
  else if (texID == 3)
    color = texture(tex3, texcoord).rgb;  
  else if (texID == 4)
    color = texture(tex4, texcoord).rgb;  
  else if (texID == 5)
    color = texture(tex5, texcoord).rgb; 
  else{
           outColor = vec4(1,0,0,1);
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
  float mag = 0.3*min(oColor.r, 1) + 0.6*min(oColor.g, 1) + 0.1*min(oColor.b, 1);
  //float mag = min(oColor.r, 1) + min(oColor.g, 1) + min(oColor.b, 1);
  if(mag < 0.4) {
      oColor = (0.4/mag)*oColor;
     //oColor = vec3(0,1,0);
  }
  else if(mag < 0.8) {
      oColor = (0.8/mag)*oColor;
     //oColor = vec3(0,0,1);
  }
  else {
     //outColor = vec4(1, 1, 1, 1);
      oColor = (0.8/mag)*oColor;
     //oColor = vec3(1,0,0);
  }

  //control flow to prevent drawing outline for some models
  
  if (UIRender == 1) {
      outColor += vec4(color,1);
      // outColor += vec4(textureOffset(texID, texcoord, texOffset), 1);
  }
  if  (drawOutline == 0) {
      outColor += vec4(oColor.r, oColor.g, oColor.b, 1);
  }
  else {
      if(dot(normal,viewDir) < 0.32) outColor += vec4(0,0,0,1);
      // else if(dot(normal,lightDir) > 0.9999) outColor = vec4(1,1,1,1);
      else (outColor += vec4(oColor.r, oColor.g, oColor.b, 1));
      // outColor = vec4(0, 0, 1, 1);
  }
  

  //outColor = vec4(oColor,1);   // commented out from original by NoW @ 11/22/14 @ 5:10 PM
}