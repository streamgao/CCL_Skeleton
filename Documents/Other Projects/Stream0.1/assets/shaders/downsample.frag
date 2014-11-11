/*
Copyright (c) 2014, Paul Houx - All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/*
This shader performs downsampling of a texture to 1/16th of its size in a single pass.
Upon resizing, the brightest pixel of a block of 4x4 pixels is preserved. 

In order for this shader to work:
  - use 2 frame buffers in total ('ping' & 'pong')
  - render source frame to 'ping' buffer using this shader,
    and make sure to use premultipied alpha blending.
  - render the result ('ping') to the 'pong' buffer while applying
    horizontal blurring.
  - render the result ('pong') back to the 'ping' buffer while
    applying vertical blurring.

Uniforms:
  - 'uExtends' should be set to vec4( 1/w, 1/h, 0, 0), 
    where w is the width of the source image in pixels 
    and h is the height of the source image in pixels.
  - 'uAfterImage' is a value in the range [0, 1]. 
    If set larger than zero, the shader will create an
    after-image effect, where some of the blooming of 
    the previous frame is preserved. This will increase
    the effect at no extra cost and reduce flickering,
    but should be used with care.
*/

#version 130

uniform sampler2D uTex;
uniform float     uAfterImage;

in vec4 vTexCoord0;
in vec4 vTexCoord1;

out vec4 oColor;

void main( void )
{
	// Conversion to luminance.
	const vec3 luminance = vec3( 0.2126, 0.7152, 0.0722 );

	// calculate luminance of brightest texel
	vec3 col0 = texture( uTex, vTexCoord0.xy ).rgb;
	vec3 col1 = texture( uTex, vTexCoord0.zw ).rgb;
	vec3 col2 = texture( uTex, vTexCoord1.xy ).rgb;
	vec3 col3 = texture( uTex, vTexCoord1.zw ).rgb;

	float lum0 = dot( luminance, col0 );
	float lum1 = dot( luminance, col1 );
	float lum2 = dot( luminance, col2 );
	float lum3 = dot( luminance, col3 );

	float lum = max( max( lum0, lum1 ), max( lum2, lum3 ) );

	// If below a certain threshold, set to black.
	if( lum < 1.0 ) {
		oColor = vec4( 0, 0, 0, 1.0 - uAfterImage );
	}
	else {
		// Keep the brightest texel of the four.
		oColor.rgb = step( lum, lum0 ) * col0;
		oColor.rgb += step( lum, lum1 ) * col1;
		oColor.rgb += step( lum, lum2 ) * col2;
		oColor.rgb += step( lum, lum3 ) * col3;

		oColor.a = 1.0;
	}
}
