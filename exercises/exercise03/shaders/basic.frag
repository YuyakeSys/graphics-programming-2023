#version 330 core

// (todo) 03.X: Add the in variables here
in vec4 Normal;
in vec3 NewPoisiton;

out vec4 FragColor;

uniform vec3 Color = vec3(1);

void main()
{
	//FragColor = vec4(Color, 1) * dot(normalize(Normal.xyz),vec3(1));
	FragColor = vec4(NewPoisiton, 1) * dot(normalize(Normal.xyz),vec3(1));
}


