#version 130

uniform mat4 ciModelMatrix;
uniform mat4 ciViewMatrix;
uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;

uniform mat4 uShadowMatrix;

in vec4 ciPosition;
in vec3 ciNormal;

out vec4 vWorldPosition;
out vec4 vViewPosition;
out vec3 vNormal;
out vec4 vShadowCoord;

void main( void )
{
	// We need the position of the vertex in world space
	// for shadow mapping and light absorption.
	vWorldPosition = ciModelMatrix * ciPosition;

	// We need the position of the vertex in view space
	// for the lighting equations.
	vViewPosition = ciViewMatrix * vWorldPosition;

	// We need the normal for the lighting equations.
	vNormal = ciNormalMatrix * ciNormal;

	// We need the position of the vertex in shadow space
	// for the shadow map calculations.
	vShadowCoord = uShadowMatrix * vWorldPosition;

	// And we must always output the vertex in clip space.
	gl_Position = ciModelViewProjection * ciPosition;
}