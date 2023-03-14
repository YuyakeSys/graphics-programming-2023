#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D Texture2DLoader;
uniform vec3 AmbientColor;
uniform float AmbientReflection;
uniform float DiffuseReflection;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform float SpecularExponent;

vec3 GetAmbientReflection(vec4 color)
{
	return color.rgb * AmbientColor * AmbientReflection;
}

vec3 GetDiffuseReflection(vec4 color, vec3 norm, vec3 lightColor,  vec3 lightDirection)
{
	float dotProduct = max(dot(norm, lightDirection), 0);
	return color.rgb * DiffuseReflection * LightColor * dotProduct;
}

vec3 GetSpecularReflection(vec4 color, vec3 norm, vec3 lightColor,  vec3 lightDirection)
{
	float dotProduct = max(dot(norm, lightDirection), 0);
	return color.rgb * DiffuseReflection * LightColor * dotProduct;
}

vec3 GetBlinnPhongReflection(vec4 color, vec3 norm, vec3 lightColor, vec3 lightDirection)
{
	return (GetAmbientReflection(color) + GetDiffuseReflection(color, norm, lightColor, lightDirection));
}


void main()
{
	vec4 color = Color * texture(Texture2DLoader, TexCoord);
	vec3 norm = normalize(WorldNormal);
	vec3 lightDir = normalize(LightPosition - WorldPosition);
	FragColor = vec4(GetBlinnPhongReflection(color,norm, LightColor, lightDir),1.0);
}
