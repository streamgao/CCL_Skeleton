#version 130

uniform sampler2D uTexScene;
uniform sampler2D uTexBloom;
uniform vec4 uColorScene;
uniform vec4 uColorBloom;

in vec2 vTexCoord0;
in vec4 vColor;

out vec4 oColor;

// For more information on tone mapping and blooming, see:
//  http://frictionalgames.blogspot.com/2012/09/tech-feature-hdr-lightning.html
vec3 filmicToneMapping( in vec3 x )
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;

	return ( ( x * ( A * x + C * B ) + D * E ) / ( x * ( A * x + B ) + D * F ) ) - E / F;
}

void main( void )
{
	// Additive blend the blooming pass.
	oColor = texture( uTexScene, vTexCoord0 ) * uColorScene;
	oColor += texture( uTexBloom, vTexCoord0 ) * uColorBloom;

	// Perform filmic tone mapping.
	const float exposure = 1.0;
	float whitepoint = 1.0 / filmicToneMapping( vec3( 1.0 ) );
	oColor.rgb = vColor.rgb * filmicToneMapping( oColor.rgb * exposure ) * whitepoint;

	// Output gamma corrected final color.
	oColor.rgb = sqrt( oColor.rgb );
	oColor.a = 1.0;
}