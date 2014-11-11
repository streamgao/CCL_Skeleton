#version 130

uniform sampler2D uTex;
uniform vec4 uExtends;

in vec2 vTexCoord0;

out vec4 oColor;

void main( void )
{
	// To create your own Gaussian kernels, see:
	//  http://dev.theomader.com/gaussian-kernel-calculator/

	// sigma 5.0, kernel size 13
	oColor.rgb = 0.048196 * texture( uTex, vTexCoord0.xy - 6.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.060013 * texture( uTex, vTexCoord0.xy - 5.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.071805 * texture( uTex, vTexCoord0.xy - 4.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.082557 * texture( uTex, vTexCoord0.xy - 3.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.091209 * texture( uTex, vTexCoord0.xy - 2.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.09683 * texture( uTex, vTexCoord0.xy - 1.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.098779 * texture( uTex, vTexCoord0.xy ).rgb;
	oColor.rgb += 0.09683 * texture( uTex, vTexCoord0.xy + 1.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.091209 * texture( uTex, vTexCoord0.xy + 2.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.082557 * texture( uTex, vTexCoord0.xy + 3.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.071805 * texture( uTex, vTexCoord0.xy + 4.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.060013 * texture( uTex, vTexCoord0.xy + 5.0 * uExtends.xy ).rgb;
	oColor.rgb += 0.048196 * texture( uTex, vTexCoord0.xy + 6.0 * uExtends.xy ).rgb;
	//*/

	oColor.a = 1.0;
}
