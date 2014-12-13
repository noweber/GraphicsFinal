#version 150 core

in vec3 position;
//in vec3 inColor;

//const vec3 inColor = vec3(0.f,0.7f,0.f);

uniform int lightOffset;	// Now 11/18/2014 @ 3:44 AM
vec3 inLightDir = normalize(vec3(lightOffset,-1,0));


in vec3 inNormal;
in vec2 inTexcoord;

out vec3 Color;
out vec3 normal;
out vec3 pos;
out vec3 lightDir;
out vec2 texcoord;

// For the "ground" I wanted to draw... NoW 11/28/14 @ 3:41 AM
uniform mat4 ground;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 inColor;

void main() {
  Color = inColor;
  gl_Position = proj * view * model * vec4(position, 1.0);
  pos = (view * model * vec4(position, 1.0)).xyz;
  lightDir = (view * vec4(inLightDir, 0.0)).xyz;
  vec4 norm4 = transpose(inverse(view*model)) * vec4(inNormal, 0.0);
  normal = normalize(norm4.xyz);

  //Invert V coordinate to support UV Map exported by Blender
  texcoord = vec2(inTexcoord.x, 1-inTexcoord.y);
}