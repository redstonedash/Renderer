//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//out vec3 vertColor;
//void main()
//{
//	gl_Position = vec4(aPos, 1.0);
//	vertColor = aColor;
//}

#version 430
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 uv;
layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 model;
layout(location = 4) uniform vec3 lightPos;
out vec2 vUV;
out vec4 vertColor;
void main() { gl_Position = proj * view * model * position; vUV = uv; 
vertColor = vec4(dot((model * vec4(normal.xyz,0)).xyz, (model * position).xyz-lightPos).xxx,1);
}

//#version 410
//layout(location = 0) in vec4 position;
//void main()
//{
//	gl_Position = position;
//}
//
// vertex shader
// this is run for each vertex on a mesh	