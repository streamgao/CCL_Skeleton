#version 130

uniform sampler2D uTexScene;
uniform sampler2D uTexBloom;

in vec2 vTexCoord0;
in vec4 vColor;

out vec4 oColor;

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
	const float exposure = 1.0;
	float whitepoint = 1.0 / filmicToneMapping( vec3( 1.0 ) );

	oColor = texture( uTexScene, vTexCoord0 );
	oColor += texture( uTexBloom, vTexCoord0 ) * vColor;

	oColor.rgb = filmicToneMapping( oColor.rgb * exposure ) * whitepoint;
}