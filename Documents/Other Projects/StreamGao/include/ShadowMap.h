/*
Copyright (c) 2014, Eric Renaud-Houde - All rights reserved.
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

#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"

typedef std::shared_ptr<class ShadowMap> ShadowMapRef;

class ShadowMap {
public:
	static ShadowMapRef create( int size ) { return ShadowMapRef( new ShadowMap{ size } ); }
	ShadowMap( int size )
	{
		reset( size );
	}

	void reset( int size )
	{
		ci::gl::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
		depthFormat.setMagFilter( GL_LINEAR );
		depthFormat.setMinFilter( GL_LINEAR );
		depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		depthFormat.setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
		depthFormat.setCompareFunc( GL_LEQUAL );
		depthFormat.setPixelDataFormat( GL_DEPTH_COMPONENT );
		depthFormat.setPixelDataType( GL_FLOAT );
		mTextureShadowMap = ci::gl::Texture2d::create( size, size, depthFormat );

		ci::gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_DEPTH_ATTACHMENT, mTextureShadowMap );
		mShadowMap = ci::gl::Fbo::create( size, size, fboFormat );
	}

	const ci::gl::FboRef&		getFbo() const { return mShadowMap; }
	const ci::gl::Texture2dRef&	getTexture() const { return mTextureShadowMap; }

	float					getAspectRatio() const { return mShadowMap->getAspectRatio(); }
	ci::ivec2				getSize() const { return mShadowMap->getSize(); }
private:
	ci::gl::FboRef			mShadowMap;
	ci::gl::Texture2dRef	mTextureShadowMap;
};