/*
Copyright (c) 2014, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "Resources.h"

#include "Bloom.h"
#include "cinder/app/AppBasic.h"

using namespace ci;
using namespace ci::app;

void Bloom::setup()
{
	try {
#if USE_RESOURCES
		mShaderDownsample = gl::GlslProg::create( loadResource( RES_VERT_DOWNSAMPLE ), loadResource( RES_FRAG_DOWNSAMPLE ) );
		mShaderBlur = gl::GlslProg::create( loadResource( RES_VERT_BLUR ), loadResource( RES_FRAG_BLUR ) );
#else
		mShaderDownsample = gl::GlslProg::create( loadAsset( "shaders/downsample.vert" ), loadAsset( "shaders/downsample.frag" ) );
		mShaderBlur = gl::GlslProg::create( loadAsset( "shaders/blur.vert" ), loadAsset( "shaders/blur.frag" ) );
#endif
	}
	catch( const std::exception& e ) {
		console() << e.what() << std::endl;
	}
}

void Bloom::resize( const ivec2& size )
{
	gl::Texture2d::Format tfmt;
	tfmt.setMinFilter( GL_LINEAR );
	tfmt.setMagFilter( GL_LINEAR );
	tfmt.setInternalFormat( GL_RGB16F );

	gl::Fbo::Format fmt;
	fmt.disableDepth();
	fmt.colorTexture( tfmt );

	mFbo[0] = gl::Fbo::create( size.x / 4, size.y / 4, fmt );
	mFbo[1] = gl::Fbo::create( size.x / 4, size.y / 4, fmt );
}

gl::Texture2dRef Bloom::bloom( const gl::Texture2dRef& source )
{
	int w = source->getWidth();
	int h = source->getHeight();

	gl::pushMatrices();

	// downsample and brightness threshold
	{
		gl::ScopedFramebuffer fbo( mFbo[mFboPing] );
		gl::ScopedTextureBind tex0( source );
		gl::ScopedGlslProg shader( mShaderDownsample );
		gl::ScopedViewport viewport( ivec2( 0 ), mFbo[mFboPing]->getSize() );

		mShaderDownsample->uniform( "uTex", 0 );
		mShaderDownsample->uniform( "uAfterImage", mAfterImage );
		mShaderDownsample->uniform( "uExtends", vec4( 1.0f / w, 1.0f / h, w, h ) );

		gl::setMatricesWindow( mFbo[mFboPing]->getSize() );

		gl::enableAlphaBlending( true );
		gl::drawSolidRect( mFbo[mFboPing]->getBounds() );
		gl::disableAlphaBlending();
	}

	w = mFbo[mFboPing]->getWidth();
	h = mFbo[mFboPing]->getHeight();

	for( size_t i = 0; i < mNumBlurPasses; ++i ) {
		// horizontal blur
		if( true ) {
			gl::ScopedFramebuffer fbo( mFbo[mFboPong] );
			gl::ScopedTextureBind tex0( mFbo[mFboPing]->getColorTexture() );
			gl::ScopedGlslProg shader( mShaderBlur );
			gl::ScopedViewport viewport( ivec2( 0 ), mFbo[mFboPong]->getSize() );

			mShaderBlur->uniform( "uExtends", vec4( 1.0f / w, 0.0f, w, h ) );

			gl::setMatricesWindow( mFbo[mFboPong]->getSize() );
			gl::drawSolidRect( mFbo[mFboPong]->getBounds() );
		}

		// vertical blur
		if( true ) {
			gl::ScopedFramebuffer fbo( mFbo[mFboPing] );
			gl::ScopedTextureBind tex0( mFbo[mFboPong]->getColorTexture() );
			gl::ScopedGlslProg shader( mShaderBlur );
			gl::ScopedViewport viewport( ivec2( 0 ), mFbo[mFboPing]->getSize() );

			mShaderBlur->uniform( "uExtends", vec4( 0.0f, 1.0f / h, w, h ) );

			gl::setMatricesWindow( mFbo[mFboPing]->getSize() );
			gl::drawSolidRect( mFbo[mFboPing]->getBounds() );
		}
	}

	gl::popMatrices();

	return mFbo[mFboPing]->getColorTexture();
}