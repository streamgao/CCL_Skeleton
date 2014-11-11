#version 330

in vec4 ciPosition;
in vec3 ciNormal;

uniform mat4 ciModelMatrix;
uniform mat4 ciViewMatrix;
uniform mat3 ciNormalMatrix;
uniform mat4 ciProjectionMatrix;
uniform mat4 ciModelViewProjection;

uniform mat4 uShadowMatrix;

out vec3 vNormal;  // calculate the normal
out vec4 vPositionInWorldSpace; // the position of the pixel in the world
out vec4 vPositionInCameraSpace; // the position of the pixel as seen from the camera
out vec4 vShadowMapCoord;

void main() {
    
    // We need the position of the vertex in world space
    // for shadow mapping and light absorption.
    vPositionInWorldSpace = ciModelMatrix * ciPosition;
    
    // We need the position of the vertex in view space
    // for the lighting equations.
    vPositionInCameraSpace = ciViewMatrix * vPositionInWorldSpace;
    
    // We need the normal for the lighting equations.
    vNormal = ciNormalMatrix * ciNormal;
    
    // We need the position of the vertex in shadow space
    // for the shadow map calculations.
    vShadowMapCoord = uShadowMatrix * vPositionInWorldSpace;
    
    gl_Position = ciModelViewProjection * ciPosition;   //display on the screen
}
