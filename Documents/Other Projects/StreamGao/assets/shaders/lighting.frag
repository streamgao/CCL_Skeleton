#version 130

uniform sampler2DShadow  uShadowMap;

uniform mat4             uLightViewMatrix;

uniform vec3             uLightColor;
uniform vec4             uLightViewPosition;
uniform vec4             uLightViewDirection;
uniform vec4             uLightParams; // 1/range, -, -, -

uniform vec3             uAmbientSkyColor;
uniform vec3             uAmbientGroundColor;
uniform vec4             uSkyViewDirection;
uniform float            uAmbientStrength;

uniform float            uDiffuseStrength;

uniform float            uSpecularStrength;
uniform float            uSpecularShininess;

uniform float            uRimStrength;
uniform float            uTranslucentStrength;

uniform float            uShadowStrength;

in vec4 vWorldPosition;
in vec4 vViewPosition;
in vec3 vNormal;
in vec4 vShadowCoord;

out vec4 oColor;

const vec3  kDiffuseColor = vec3( 0.1, 0.1, 0.1 );
const vec3  kSpecularColor = vec3( 0.25, 0.25, 0.25 );

float sampleBasic( vec4 sc )
{
	return textureProj( uShadowMap, sc );
}

float samplePCF4x4( vec4 sc )
{
	const int r = 1;
	const int s = 2 * r;

	float shadow = 0.0;
	shadow += textureProjOffset( uShadowMap, sc, ivec2( -s, -s ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( -r, -s ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( r, -s ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( s, -s ) );

	shadow += textureProjOffset( uShadowMap, sc, ivec2( -s, -r ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( -r, -r ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( r, -r ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( s, -r ) );

	shadow += textureProjOffset( uShadowMap, sc, ivec2( -s, r ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( -r, r ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( r, r ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( s, r ) );

	shadow += textureProjOffset( uShadowMap, sc, ivec2( -s, s ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( -r, s ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( r, s ) );
	shadow += textureProjOffset( uShadowMap, sc, ivec2( s, s ) );

	return shadow / 16.0;
}

void main( void )
{
	// Calculate lighting vectors.
	vec3 N = normalize( vNormal );
	vec3 E = normalize( -vViewPosition.xyz );
	vec3 L = normalize( uLightViewPosition.xyz - vViewPosition.xyz );
	vec3 R = normalize( -reflect( L, N ) );

	// Light distance attenuation.
	float dist2 = dot( uLightViewPosition.xyz, vViewPosition.xyz );
	float dist = sqrt( dist2 );
	float attenuation = clamp( 1.0 - dist * uLightParams.x, 0.0, 1.0 );

	// Lambert term (NdotL).
	float lambert = dot( N, L );

	// Hemispherical ambient lighting.
	float hemi = dot( N, uSkyViewDirection.xyz ) * 0.5 + 0.5;
	vec3 ambient = uAmbientStrength * mix( uAmbientGroundColor, uAmbientSkyColor, hemi );

	// Shadows.
	float shadow = mix( 1.0f, samplePCF4x4( vShadowCoord ), uShadowStrength );

	// Translucent light (absorption).
	float contribution = 0.75 - 0.25 * dot( -uLightViewDirection.xyz, E );
	float dLightToBackSide = textureProj( uShadowMap, vShadowCoord ) * 0.01;
	float dLightToFrontSide = length( uLightViewMatrix * vWorldPosition );
	float absorption = pow( abs( dLightToFrontSide - dLightToBackSide ) * 0.25, 4.0 );
	vec3 translucent = uTranslucentStrength * contribution * pow( exp( -absorption ), 4.0 ) * ( uLightColor + ambient );

	// Rim lighting.
	vec3 rim = vec3( 0 );
	rim += pow( vec3( 1.0 - max( dot( N, E ), 0.0 ) ), vec3( 3.0 ) );
	rim *= 0.2 * uLightColor;
	rim *= mix( 0.5, 1.0, shadow );
	rim *= uRimStrength * clamp( 1.0 - absorption, 0.0, 1.0 );

	// Diffuse lighting.
	vec3 diffuse = kDiffuseColor * uLightColor;
	diffuse *= uDiffuseStrength * max( lambert, 0.0 );

	// Energy conserving specular lighting.
	float fSpecularNorm = ( 8.0 + uSpecularShininess ) / ( 8.0 * 3.14159265 );
	vec3 specular = kSpecularColor * uLightColor;
	//specular *= 0.5 + 0.5 * dot( -uLightViewDirection.xyz, E ); // Reduce specular if light is pointed towards us.
	specular *= ( lambert > 0.0 ) ? pow( max( dot( R, E ), 0.0 ), uSpecularShininess ) * fSpecularNorm : 0.0;
	specular *= uSpecularStrength;

	// Output final color.
	oColor.rgb = ambient + attenuation * ( shadow * ( specular + diffuse ) + rim + translucent );
	oColor.a = 1.0;
}
