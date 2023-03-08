#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform sampler2D RockTexture;
uniform sampler2D DirtTexture;
uniform sampler2D SnowTexture;
uniform vec2 ColorTextureScale;
uniform vec2 RangeRock;
uniform vec2 RangeDirt;
uniform vec2 RangeSnow;

float mixTextures(vec2 range, float height);

void main()
{
	vec4 grasstexel = texture(ColorTexture, TexCoord*ColorTextureScale);

    vec4 textureSnow = texture(SnowTexture, TexCoord*ColorTextureScale);
    vec4 textureRock = texture(RockTexture, TexCoord*ColorTextureScale);
    vec4 textureDirt = texture(DirtTexture, TexCoord*ColorTextureScale);

    float mix1 = mixTextures(RangeDirt, Height);
    //x×(1−a)+y×
    vec4 texel1 = textureDirt;
    texel1 = mix(texel1, grasstexel, mix1);

    texel1 = mix(texel1, textureRock, mixTextures(RangeRock, Height));

    texel1 = mix(texel1, textureSnow, mixTextures(RangeSnow, Height));

	FragColor = Color * texel1;
}

float mixTextures(vec2 range, float value) {
	return clamp((value - range.x) / (range.y - range.x), 0, 1);
}

