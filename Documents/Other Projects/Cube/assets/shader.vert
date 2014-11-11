#version 330

in vec4 ciPosition;
in vec3 ciNormal;

out vec3 vColor;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;
uniform mat4 ciProjectionMatrix;
uniform mat4 ciModelViewProjection;

vec3 ads( vec4 position, vec3 norm )
{
    
    // This is a little hack to programmatically define whether the light
    // is a directional light (LightPosition.w = 1.0) or a point light (LightPosition.w = 0.0)
    vec3 s = normalize(vec3( LightPosition - position * LightPosition.w ));
    
    vec3 v = normalize(vec3(-position));
    vec3 r = reflect( -s, norm );
    return LightIntensity * ( Ka + Kd * max( dot(s, norm), 0.0 ) + Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}

void main() {
    
    vec3 eyeNorm = normalize( ciNormalMatrix * ciNormal);
    vec4 eyePosition = ciModelView * ciPosition;
    // Evaluate the lighting equation
    vColor = ads( eyePosition, eyeNorm );
    gl_Position = ciModelViewProjection * ciPosition;
}

//#version 150
//
//uniform mat4	ciModelViewProjection;
//uniform mat3	ciNormalMatrix;
//
//in vec4		ciPosition;
//in vec2		ciTexCoord0;
//in vec3		ciNormal;
//in vec4		ciColor;
//
//out highp vec2	TexCoord;
//out lowp vec4	Color;
//out highp vec3	Normal;
//
//void main( void )
//{
//	gl_Position	= ciModelViewProjection * ciPosition;
//	Color 		= ciColor;
//	TexCoord	= ciTexCoord0;
//	Normal		= ciNormalMatrix * ciNormal;
//}
