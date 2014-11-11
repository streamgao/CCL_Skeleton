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

#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/Quaternion.h"
#include "cinder/Vector.h"

class LightSource {
public:
	LightSource()
		: mIsDirty( true )
		, mIsInverseDirty( true )
		, mIsLookingAt( false )
		, mRange( 1000 )
		, mRatio( 1 )
		, mAspect( 1 )
		, mPosition( 1 )
		, mColor( 1, 1, 1 )
	{
	}
	~LightSource() {}

	const ci::vec3& getPosition() const { return mPosition; }
	void setPosition( const ci::vec3& worldPosition )
	{
		mIsDirty = true;
		mPosition = worldPosition;
		if( mIsLookingAt )
			mDirection = glm::normalize( mLookAt - mPosition );
	}

	const ci::vec3& getDirection() const { return mDirection; }
	void setDirection( const ci::vec3& direction )
	{
		mIsDirty = true;
		mIsLookingAt = false;
		mDirection = glm::normalize( direction );
	}

	bool isLookingAt() const { return mIsLookingAt; }
	void lookAt( const ci::vec3& point )
	{
		mIsDirty = true;
		mIsLookingAt = true;
		mLookAt = point;
		mDirection = glm::normalize( mLookAt - mPosition );
	}

	float getRange() const { return mRange; }
	void setRange( float range ) { mRange = range; mIsDirty = true; }

	const ci::Color& getColor() const { return mColor; }
	//void setColor( float r, float g, float b ) { setColor( ci::Color( r, g, b ) ); }
	void setColor( const ci::Color& color ) { mColor = color; }

	void setMatrices() const;

	const ci::mat4& getViewMatrix() const { updateMatrices(); return mViewMatrix; }
	const ci::mat4& getInverseViewMatrix() const { updateInverseMatrices(); return mInverseViewMatrix; }
	const ci::mat4& getProjectionMatrix() const { updateMatrices(); return mProjectionMatrix; }
	const ci::mat4& getShadowMatrix() const { updateMatrices(); return mShadowMatrix; }

	// static functions provide easy getter/setter access
	static std::function<float()> getLightRange( const LightSource& light ) { return std::bind( &LightSource::getRange, &light ); }
	static std::function<void( float )> setLightRange( LightSource& light ) { return std::bind( &LightSource::setRange, &light, std::placeholders::_1 ); }

	static std::function<ci::ColorA()> getLightColor( const LightSource& light ) { return std::bind( &LightSource::getColor, &light ); }
	static std::function<void( ci::ColorA )> setLightColor( LightSource& light ) { return std::bind( &LightSource::setColor, &light, std::placeholders::_1 ); }

private:
	void updateMatrices() const;
	void updateInverseMatrices() const;

private:
	bool       mIsLookingAt;

	float      mRange;
	float      mRatio;
	float      mAspect;
	ci::vec3   mPosition;
	ci::vec3   mDirection;
	ci::vec3   mLookAt;

	ci::Color  mColor;

	mutable bool      mIsDirty;
	mutable bool      mIsInverseDirty;

	mutable ci::mat4  mViewMatrix;;
	mutable ci::mat4  mInverseViewMatrix;
	mutable ci::mat4  mProjectionMatrix;
	mutable ci::mat4  mShadowMatrix;
};

