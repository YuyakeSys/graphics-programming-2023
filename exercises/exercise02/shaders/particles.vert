#version 330 core

layout (location = 0) in vec2 ParticlePosition;
// (todo) 02.X: Add more vertex attributes
layout (location = 1) in float size;
layout (location = 2) in float birth;
layout (location = 3) in float duration;
layout (location = 4) in vec4 color;
layout (location = 5) in vec2 vAndG;

// (todo) 02.5: Add Color output variable here
out vec4 Color;

// (todo) 02.X: Add uniforms
uniform float CurrentTime;

float setPointSize()
{
	float size2 = size;
	if((CurrentTime-birth) > duration)
		size2 = 0.0f;
	return size2;
}

vec2 newPos()
{
	float t = CurrentTime - birth;
	vec2 v = vAndG * t;
	vec2 g = vec2(0.0f, 9.8f) * t * t * 0.5f;
	return (ParticlePosition + v + g); 
}

void main()
{
	gl_PointSize = setPointSize();
	gl_Position = vec4(newPos(), 0.0, 1.0);
	Color = color;
}
