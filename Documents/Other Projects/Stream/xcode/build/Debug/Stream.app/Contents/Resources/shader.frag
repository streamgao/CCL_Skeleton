#version 330

in vec4 vPositionInWorldSpace;
in vec4 vPositionInCameraSpace;
in vec3 vNormal;  // calculate the normal
in vec4 vShadowMapCoord;

out vec4 FragColor;

uniform vec4 uLightPosition;
uniform vec3 uLightIntensity;
uniform vec4 uLightViewMatrix;
uniform vec4 uLightProjectionMatrix;

uniform sampler2DShadow uShadowMap;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

float samplePCF4x4( vec4 sc ) // sc = shadow coordinate
{
    const int r = 1;
    const int s = 2 * r;
    
    sc.z -= 0.0005;
    
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

vec3 ads( vec4 position, vec3 norm )
{
    
    float dLightToBackSide = textureProj( uShadowMap, vShadowMapCoord );
    float dLightToFrontSide = length( uLightViewMatrix * vPositionInWorldSpace );
    float absorption = pow( abs( dLightToFrontSide - dLightToBackSide ) * 0.075, 1.0 );
    vec3 translucent = 10.0 * vec3( pow( absorption , 1.0 ));// * ( uLightColor + ambient );
    vec3 result = vec3(dLightToFrontSide);
    
    // This is a little hack to programmatically define whether the light
    // is a directional light (LightPosition.w = 1.0) or a point light (LightPosition.w = 0.0)
    vec3 s = normalize(vec3( uLightPosition - position * uLightPosition.w ));
    vec3 v = normalize(vec3(-position));
    vec3 r = reflect( -s, norm );
    
    float shadow = samplePCF4x4(vShadowMapCoord);
    
    // vec3 result = Ka; // ambient light (light that is there, even if there are no lamps)
    result += shadow * Kd * max( dot(s, norm), 0.0 ); // the light from our light source (might be shadowed)
    result += shadow * Ks * pow( max( dot(r,v), 0.0 ), Shininess ); // the shiny glossy highlights from our light source (might be shadowed)
    result *= uLightIntensity;
    
    
    return result;
}

void main() {
    
    // Evaluate the lighting equation
    vec3 N = normalize( vNormal );
    vec3 vColor = ads( vPositionInCameraSpace, N );
    
    FragColor = vec4(vColor, 1.0);
}
