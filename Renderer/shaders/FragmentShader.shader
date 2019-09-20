//#version 410
//out vec4 vertColor;
//void main()
//{
//	vertColor = vec4(1.0, 0.0, 0.0, 1.0); //red
//}
// fragment shader
// this is run for each vertex on a mesh

//#version 330 core
//out vec4 FragColor;
//uniform vec4 vertColor;
//void main()
//{
//   FragColor = vertColor;
//}

#version 430
layout (location = 3) uniform sampler2D albedo;
in vec2 vUV;
in vec4 vertColor;
out vec4 fragColor;
void main() { fragColor = texture(albedo, vUV)*vertColor;}