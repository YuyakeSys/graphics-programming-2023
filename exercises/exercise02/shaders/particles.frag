#version 330 core

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here
in vec4 Color;

void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord

	float alpha = 1 - length(2 * gl_PointCoord - 1);
	FragColor = vec4(Color.r, Color.g, Color.b, alpha);
}
