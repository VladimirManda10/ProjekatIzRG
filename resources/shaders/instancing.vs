#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aOff;

uniform mat4 projection;
uniform mat4 view;
out vec3 fColor;

void main(){
	fColor = aColor;
	gl_Position = projection*view*vec4(aPos.x + aOff.x,aPos.y + aOff.y,aPos.z + aOff.z,1.0f);
}