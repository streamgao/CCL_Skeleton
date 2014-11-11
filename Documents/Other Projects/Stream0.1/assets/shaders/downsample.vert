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

uniform mat4 ciModelViewProjection;
uniform vec4 uExtends;

in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec4 vTexCoord0;
out vec4 vTexCoord1;

void main( void )
{
	// Generate texture coordinates to quickly evaluate 4 texels at once.
	vTexCoord0.xy = ciTexCoord0;
	vTexCoord0.zw = ciTexCoord0 + vec2( uExtends.x, 0.0 );
	vTexCoord1.xy = ciTexCoord0 + vec2( 0.0, uExtends.y );
	vTexCoord1.zw = ciTexCoord0 + vec2( uExtends.x, uExtends.y);

	gl_Position = ciModelViewProjection * ciPosition;
}