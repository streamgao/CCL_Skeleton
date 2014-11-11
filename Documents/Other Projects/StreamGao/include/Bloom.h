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

#pragma once

#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

class Bloom {
public:
	Bloom() : mFboPing( 0 ), mFboPong( 1 ), mNumBlurPasses( 4 ), mAfterImage( 0.0f ) {}
	~Bloom() {}

	void setup();
	void resize( const ci::ivec2& size );

	ci::gl::Texture2dRef bloom( const ci::gl::Texture2dRef& source );

	size_t getNumBlurPasses() const { return mNumBlurPasses; }
	void setNumBlurPasses( size_t num ) { mNumBlurPasses = num; }

	float getAfterImage() const { return mAfterImage; }
	void setAfterImage( float value ) { mAfterImage = ci::math<float>::clamp( value, 0.0f, 1.0f ); }

	// static functions provide easy getter/setter access
	static std::function<float()> getBloomAfterImage( const Bloom& bloom ) { return std::bind( &Bloom::getAfterImage, &bloom ); }
	static std::function<void( float )> setBloomAfterImage( Bloom& bloom ) { return std::bind( &Bloom::setAfterImage, &bloom, std::placeholders::_1 ); }

private:
	ci::gl::GlslProgRef  mShaderDownsample;
	ci::gl::GlslProgRef  mShaderBlur;

	ci::gl::FboRef   mFbo[2];
	size_t           mFboPing, mFboPong;

	size_t           mNumBlurPasses;

	float            mAfterImage;
};

