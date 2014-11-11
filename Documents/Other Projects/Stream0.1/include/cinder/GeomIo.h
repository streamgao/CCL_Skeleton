/*
 Copyright (c) 2013, The Cinder Project, All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/Exception.h"
#include "cinder/Vector.h"
#include "cinder/Matrix.h"
#include "cinder/Shape2d.h"
#include "cinder/Color.h"

#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <array>

// Forward declarations in cinder::
namespace cinder {
	class TriMesh;
	template<int D, typename T>
	class BSpline;
}

namespace cinder { namespace geom {

class Target;
typedef std::shared_ptr<class Source>	SourceRef;

// keep this incrementing by 1 only; some code relies on that for iterating; add corresponding entry to sAttribNames
enum Attrib { POSITION, COLOR, TEX_COORD_0, TEX_COORD_1, TEX_COORD_2, TEX_COORD_3,
	NORMAL, TANGENT, BITANGENT, BONE_INDEX, BONE_WEIGHT, 
	CUSTOM_0, CUSTOM_1, CUSTOM_2, CUSTOM_3, CUSTOM_4, CUSTOM_5, CUSTOM_6, CUSTOM_7, CUSTOM_8, CUSTOM_9,
	NUM_ATTRIBS };
extern std::string sAttribNames[(int)Attrib::NUM_ATTRIBS];
enum Primitive { LINES, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN };
enum DataType { FLOAT, INTEGER, DOUBLE };

//! Debug utility which returns the name of \a attrib as a std::string
std::string attribToString( Attrib attrib );

struct AttribInfo {
	AttribInfo( const Attrib &attrib, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 )
		: mAttrib( attrib ), mDims( dims ), mDataType( DataType::FLOAT ), mStride( stride ), mOffset( offset ), mInstanceDivisor( instanceDivisor )
	{}
	AttribInfo( const Attrib &attrib, DataType dataType, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 )
		: mAttrib( attrib ), mDims( dims ), mDataType( dataType ), mStride( stride ), mOffset( offset ), mInstanceDivisor( instanceDivisor )
	{}

	Attrib		getAttrib() const { return mAttrib; }
	uint8_t		getDims() const { return mDims; }
	DataType	getDataType() const { return mDataType; }
	size_t		getStride() const { return mStride; }
	size_t		getOffset() const { return mOffset;	}
	uint32_t	getInstanceDivisor() const { return mInstanceDivisor; }

	void		setStride( size_t stride ) { mStride = stride; }
	void		setOffset( size_t offset ) { mOffset = offset; }

	uint8_t		getByteSize() const { if( mDataType == geom::DataType::DOUBLE ) return mDims * 8; else return mDims * 4; }

  protected:
	Attrib		mAttrib;
	DataType	mDataType;
	int32_t		mDims;
	size_t		mStride;
	size_t		mOffset;
	uint32_t	mInstanceDivisor;
}; 


class BufferLayout {
  public:
	BufferLayout() {}
	BufferLayout( const std::vector<AttribInfo> &attribs )
		: mAttribs( attribs )
	{}

	void append( const Attrib &attrib, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 ) {
		mAttribs.push_back( AttribInfo( attrib, DataType::FLOAT, dims, stride, offset, instanceDivisor ) );
	}
	void append( const Attrib &attrib, DataType dataType, uint8_t dims, size_t stride, size_t offset, uint32_t instanceDivisor = 0 ) {
		mAttribs.push_back( AttribInfo( attrib, dataType, dims, stride, offset, instanceDivisor ) );
	}

	//! Returns the AttribInfo for a given Attrib, and throws ExcMissingAttrib if it is not available
	AttribInfo		getAttribInfo( Attrib attrib ) const;
	//! Returns whether a given Attrib is present in the BufferLayout
	bool			hasAttrib( Attrib attrib ) const;
	//! Returns the dimensions for a given Attrib, or 0 if it is not in the BufferLayout
	uint8_t			getAttribDims( Attrib attrib ) const;
	//! Returns a vector of all present Attribs
	const std::vector<AttribInfo>&	getAttribs() const { return mAttribs; }
  protected:
	std::vector<AttribInfo>		mAttribs;
};

void copyData( uint8_t srcDimensions, const float *srcData, size_t numElements, uint8_t dstDimensions, size_t dstStrideBytes, float *dstData );

class Source {
  public:
	virtual size_t		getNumVertices() const = 0;
	virtual size_t		getNumIndices() const = 0;
	virtual Primitive	getPrimitive() const = 0;
	virtual uint8_t		getAttribDims( Attrib attr ) const = 0;

	virtual void		loadInto( Target *target ) const = 0;

	virtual void		clearAttribs() { mEnabledAttribs.clear(); }
	virtual Source&		enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Source&		disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }
	virtual bool		isEnabled( Attrib attrib ) const;

  protected:
	//! Builds a sequential list of vertices to simulate an indexed geometry when Source is non-indexed. Assumes \a dest contains storage for getNumVertices() entries
	void	copyIndicesNonIndexed( uint16_t *dest ) const;
	//! Builds a sequential list of vertices to simulate an indexed geometry when Source is non-indexed. Assumes \a dest contains storage for getNumVertices() entries
	void	copyIndicesNonIndexed( uint32_t *dest ) const;
	template<typename T>
	void forceCopyIndicesTrianglesImpl( T *dest ) const;

	std::set<Attrib>	mEnabledAttribs;
};

class Target {
  public:
	virtual uint8_t		getAttribDims( Attrib attr ) const = 0;	

	virtual void	copyAttrib( Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) = 0;
	virtual void	copyIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) = 0;

	//! For non-indexed geometry, this generates appropriate indices and then calls the copyIndices() virtual method.
	void	generateIndices( Primitive sourcePrimitive, size_t sourceNumIndices );

  protected:
	void copyIndexData( const uint32_t *source, size_t numIndices, uint32_t *target );
	void copyIndexData( const uint32_t *source, size_t numIndices, uint16_t *target );
	void copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint32_t *target );
	void copyIndexDataForceTriangles( Primitive primitive, const uint32_t *source, size_t numIndices, uint16_t *target );
};

class Rect : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Equivalent to Rectf( -0.5, -0.5, 0.5, 0.5 )
	Rect();
	Rect( const Rectf &r );

	virtual Rect&		enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Rect&		disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }	
	Rect&				rect( const Rectf &r );
	//! Enables COLOR attrib and specifies corner values in clockwise order starting with the upper-left
	Rect&				colors( const ColorAf &upperLeft, const ColorAf &upperRight, const ColorAf &lowerRight, const ColorAf &lowerLeft );
	//! Enables TEX_COORD_0 attrib and specifies corner values in clockwise order starting with the upper-left
	Rect&				texCoords( const vec2 &upperLeft, const vec2 &upperRight, const vec2 &lowerRight, const vec2 &lowerLeft );

	virtual size_t		getNumVertices() const override { return 4; }
	virtual size_t		getNumIndices() const override { return 0; }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLE_STRIP; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	std::array<vec2,4>		mPositions, mTexCoords;
	std::array<ColorAf,4>	mColors;

	static float	sNormals[4*3];
};

class Cube : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL
	Cube();

	virtual Cube&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Cube&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }
	Cube&			subdivisions( int sub ) { mSubdivisions = ivec3( std::max<int>( 1, sub ) ); return *this; }
	Cube&			subdivisionsX( int sub ) { mSubdivisions.x = std::max<int>( 1, sub ); return *this; }
	Cube&			subdivisionsY( int sub ) { mSubdivisions.y = std::max<int>( 1, sub ); return *this; }
	Cube&			subdivisionsZ( int sub ) { mSubdivisions.z = std::max<int>( 1, sub ); return *this; }
	Cube&			size( const vec3 &sz ) { mSize = sz; return *this; }
	Cube&			size( float x, float y, float z ) { mSize = vec3( x, y, z ); return *this; }

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	ivec3		mSubdivisions;
	vec3		mSize;	
};

class Icosahedron : public Source {
  public:
	//! Defaults to having POSITION and NORMAL. Supports COLOR
	Icosahedron();
	virtual ~Icosahedron() {}

	virtual Icosahedron&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Icosahedron&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override { calculate(); return mPositions.size(); }
	virtual size_t		getNumIndices() const override { calculate(); return mIndices.size(); }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	virtual void		calculate() const;

	mutable bool						mCalculationsCached;
	mutable std::vector<vec3>			mPositions;
	mutable std::vector<vec3>			mNormals;
	mutable std::vector<vec3>			mColors;
	mutable std::vector<uint32_t>		mIndices;

	static float	sPositions[12*3];

	static uint32_t	sIndices[60];
};

class Teapot : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL
	Teapot();

	virtual Teapot&		enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Teapot&		disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Teapot&				subdivisions( int sub );

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual void		loadInto( Target *target ) const override;
	virtual uint8_t		getAttribDims( Attrib attr ) const override;

  protected:
	void			calculate() const;
	void			updateVertexCounts() const;

	static void		generatePatches( float *v, float *n, float *tc, uint32_t *el, int grid );
	static void		buildPatchReflect( int patchNum, float *B, float *dB, float *v, float *n, float *tc, unsigned int *el,
		int &index, int &elIndex, int &tcIndex, int grid, bool reflectX, bool reflectY );
	static void		buildPatch( vec3 patch[][4], float *B, float *dB, float *v, float *n, float *tc, 
		unsigned int *el, int &index, int &elIndex, int &tcIndex, int grid, const mat3 reflect, bool invertNormal );
	static void		getPatch( int patchNum, vec3 patch[][4], bool reverseV );
	static void		computeBasisFunctions( float *B, float *dB, int grid );
	static vec3	evaluate( int gridU, int gridV, const float *B, const vec3 patch[][4] );
	static vec3	evaluateNormal( int gridU, int gridV, const float *B, const float *dB, const vec3 patch[][4] );

	int			mSubdivision;

	mutable bool					mCalculationsCached;
	mutable	size_t					mNumVertices;
	mutable size_t					mNumIndices;
	mutable std::vector<float>		mPositions;
	mutable std::vector<float>		mTexCoords;
	mutable std::vector<float>		mNormals;
	mutable std::vector<uint32_t>	mIndices;

	static const uint8_t	sPatchIndices[][16];
	static const float		sCurveData[][3];
};

class Circle : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL
	Circle();

	virtual Circle&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Circle&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }
	Circle&		center( const vec2 &center ) { mCenter = center; return *this; }
	Circle&		radius( float radius );
	Circle&		subdivisions( int subdivs );

	virtual void		loadInto( Target *target ) const override;
	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override { return 0; }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLE_FAN; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;

  private:
	void	updateVertexCounts();
	void	calculate() const;

	vec2		mCenter;
	float		mRadius;
	int			mRequestedSubdivisions, mNumSubdivisions;

	size_t						mNumVertices;
	mutable std::vector<vec2>	mPositions;
	mutable std::vector<vec2>	mTexCoords;
	mutable std::vector<vec3>	mNormals;
};

class Sphere : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Sphere();
	virtual ~Sphere() {}

	virtual Sphere&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Sphere&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Sphere&		center( const vec3 &center ) { mCenter = center; mCalculationsCached = false; return *this; }
	Sphere&		radius( float radius ) { mRadius = radius; mCalculationsCached = false; return *this; }
	//! Specifies the number of segments, which determines the roundness of the sphere.
	Sphere&		subdivisions( int subdiv ) { mSubdivisions = subdiv; mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	virtual void		calculate() const;
	virtual void		calculateImplUV( size_t segments, size_t rings ) const;

	vec3		mCenter;
	float		mRadius;
	int			mSubdivisions;

	mutable bool						mCalculationsCached;
	mutable std::vector<vec3>			mPositions;
	mutable std::vector<vec2>			mTexCoords;
	mutable std::vector<vec3>			mNormals;
	mutable std::vector<vec3>			mColors;
	mutable std::vector<uint32_t>		mIndices;
};

class Icosphere : public Icosahedron {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Icosphere();

	virtual Icosphere&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Icosphere&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Icosphere&			subdivisions( int sub ) { mSubdivision = (sub > 0) ? (sub + 1) : 1; mCalculationsCached = false; return *this; }

	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	virtual void		calculate() const;
	virtual void		calculateImplUV() const;
	virtual void		subdivide() const;

	int									mSubdivision;

	mutable bool						mCalculationsCached;
	mutable std::vector<vec2>			mTexCoords;
};

class Capsule : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Capsule();

	virtual Capsule&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Capsule&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	Capsule&			center( const vec3 &center ) { mCenter = center; mCalculationsCached = false; return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the capsule. Defaults to \c 6.
	Capsule&			subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; mCalculationsCached = false; return *this; }
	//! Specifies the number of slices along the capsule's length. Defaults to \c 6. Add more subdivisions to improve texture mapping and lighting, or if you intend to bend the capsule.
	Capsule&			subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv > 1 ? subdiv : 1; mCalculationsCached = false; return *this; }
	Capsule&			radius( float radius ) { mRadius = math<float>::max(0.f, radius); mCalculationsCached = false; return *this; }
	Capsule&			length( float length ) { mLength = math<float>::max(0.f, length); mCalculationsCached = false; return *this; }
	Capsule&			direction( const vec3 &direction ) { mDirection = normalize( direction ); mCalculationsCached = false; return *this; }
	//! Conveniently sets center, length and direction
	Capsule&			set( const vec3 &from, const vec3 &to );

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  private:
	void	calculate() const;
	void	calculateImplUV( size_t segments, size_t rings ) const;
	void	calculateRing( size_t segments, float radius, float y, float dy ) const;

	vec3		mDirection, mCenter;
	float		mLength, mRadius;
	int			mSubdivisionsHeight, mSubdivisionsAxis;

	mutable bool						mCalculationsCached;
	mutable std::vector<vec3>			mPositions;
	mutable std::vector<vec2>			mTexCoords;
	mutable std::vector<vec3>			mNormals;
	mutable std::vector<vec3>			mColors;
	mutable std::vector<uint32_t>		mIndices;

};

class Torus : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Torus();
	virtual ~Torus() {}

	virtual Torus&	enable( Attrib attrib ) override { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Torus&	disable( Attrib attrib ) override { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	virtual Torus&	center( const vec3 &center ) { mCenter = center; mCalculationsCached = false; return *this; }
	virtual Torus&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; mCalculationsCached = false; return *this; }
	virtual Torus&	subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv; mCalculationsCached = false; return *this; }
	//! Allows you to twist the torus along the ring.
	virtual Torus&	twist( unsigned twist ) { mTwist = twist; mCalculationsCached = false; return *this; }
	//! Allows you to twist the torus along the ring. The \a offset is in radians.
	virtual Torus&	twist( unsigned twist, float offset ) { mTwist = twist; mTwistOffset = offset; mCalculationsCached = false; return *this; }
	//! Specifies the major and minor radius as a ratio (minor : major). Resulting torus will fit unit cube.
	virtual Torus&	ratio( float ratio ) { ratio = math<float>::clamp( ratio ); mRadiusMajor = 1; mRadiusMinor = 1 - ratio; mCalculationsCached = false; return *this; }
	//! Specifies the major and minor radius separately.
	virtual Torus&	radius( float major, float minor ) { mRadiusMajor = math<float>::max(0, major); mRadiusMinor = math<float>::max(0, minor); mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override { calculate(); return mPositions.size(); }
	virtual size_t		getNumIndices() const override { calculate(); return mIndices.size(); }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	void			calculate() const;
	void			calculateImplUV( size_t segments, size_t rings ) const;

	vec3		mCenter;
	float		mRadiusMajor;
	float		mRadiusMinor;
	int			mSubdivisionsAxis;
	int			mSubdivisionsHeight;
	float		mHeight;
	float		mCoils;
	unsigned	mTwist;
	float		mTwistOffset;

	mutable bool						mCalculationsCached;
	mutable std::vector<vec3>			mPositions;
	mutable std::vector<vec2>			mTexCoords;
	mutable std::vector<vec3>			mNormals;
	mutable std::vector<vec3>			mColors;
	mutable std::vector<uint32_t>		mIndices;
};

class Helix : public Torus {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Helix()
	{
		height(2.0f);
		coils(3.0f);
	}

	virtual Helix&	enable( Attrib attrib ) override { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Helix&	disable( Attrib attrib ) override { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	virtual Helix&	center( const vec3 &center ) override { Torus::center( center ); return *this; }
	virtual Helix&	subdivisionsAxis( int subdiv ) override { Torus::subdivisionsAxis( subdiv ); return *this; }
	virtual Helix&	subdivisionsHeight( int subdiv ) override { Torus::subdivisionsHeight( subdiv ); return *this; }
	//! Specifies the height, measured from center to center.
	Helix&			height( float height ) { mHeight = height; mCalculationsCached = false; return *this; }
	//! Specifies the number of coils.
	Helix&			coils( float coils ) { mCoils = math<float>::max(0.f, coils); mCalculationsCached = false; return *this; }
	//! Allows you to twist the helix along the ring.
	virtual Helix&	twist( unsigned twist ) override { Torus::twist( twist ); return *this; }
	//! Allows you to twist the helix along the ring. The \a offset is in radians.
	virtual Helix&	twist( unsigned twist, float offset ) override { Torus::twist( twist, offset ); return *this; }
};

class Cylinder : public Source {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Cylinder();
	virtual ~Cylinder() {}

	virtual Cylinder&	enable( Attrib attrib ) override { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Cylinder&	disable( Attrib attrib ) override { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }
	//! Specifices the base of the Cylinder.
	virtual Cylinder&	origin( const vec3 &origin ) { mOrigin = origin; mCalculationsCached = false; return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the Cylinder. Defaults to \c 18.
	virtual Cylinder&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; mCalculationsCached = false; return *this; }
	//! Specifies the number of slices along the Cylinder's height. Defaults to \c 1.
	virtual Cylinder&	subdivisionsHeight( int slices ) { mSubdivisionsHeight = slices; mCalculationsCached = false; return *this; }
	//! Specifies the height of the cylinder.
	virtual Cylinder&	height( float height ) { mHeight = height; mCalculationsCached = false; return *this; }
	//! Specifies the base and apex radius.
	virtual Cylinder&	radius( float radius ) { mRadiusBase = mRadiusApex = math<float>::max(0.f, radius); mCalculationsCached = false; return *this; }
	//! Specifies the axis of the cylinder.
	virtual Cylinder&	direction( const vec3 &direction ) { mDirection = normalize( direction );  mCalculationsCached = false; return *this; }
	//! Conveniently sets origin, height and direction so that the center of the base is \a from and the center of the apex is \a to.
	virtual Cylinder&	set( const vec3 &from, const vec3 &to );

	virtual size_t		getNumVertices() const override { calculate(); return mPositions.size(); }
	virtual size_t		getNumIndices() const override { calculate(); return mIndices.size(); }
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	virtual void	calculate() const;
	virtual void	calculateImplUV( size_t segments, size_t rings ) const;
	virtual void	calculateCap( bool flip, float height, float radius, size_t segments ) const;

	vec3		mOrigin;
	float		mHeight;
	vec3		mDirection;
	float		mRadiusBase;
	float		mRadiusApex;
	int			mSubdivisionsAxis;
	int			mSubdivisionsHeight;

	mutable bool						mCalculationsCached;
	mutable std::vector<vec3>			mPositions;
	mutable std::vector<vec2>			mTexCoords;
	mutable std::vector<vec3>			mNormals;
	mutable std::vector<vec3>			mColors;
	mutable std::vector<uint32_t>		mIndices;
};

class Cone : public Cylinder {
  public:
	//! Defaults to having POSITION, TEX_COORD_0, NORMAL. Supports COLOR
	Cone() { radius( 1.0f, 0.0f ); }
	virtual ~Cone() {}

	virtual Cone&	enable( Attrib attrib ) override { Cylinder::enable( attrib ); return *this; }
	virtual Cone&	disable( Attrib attrib ) override { Cylinder::disable( attrib ); return *this; }
	virtual Cone&	origin( const vec3 &origin ) override { Cylinder::origin( origin ); return *this; }
	//! Specifies the number of radial subdivisions, which determines the roundness of the Cone. Defaults to \c 18.
	virtual Cone&	subdivisionsAxis( int subdiv ) { mSubdivisionsAxis = subdiv; mCalculationsCached = false; return *this; }
	//! Specifies the number of subdivisions along the Cone's height. Defaults to \c 1.
	virtual Cone&	subdivisionsHeight( int subdiv ) { mSubdivisionsHeight = subdiv; mCalculationsCached = false; return *this; }
	virtual Cone&	height( float height ) override { Cylinder::height( height ); return *this; }
	//! Specifies the base and apex radius.
	virtual Cone&	radius( float radius ) override {  Cylinder::radius( radius ); return *this; }
	//! Specifies the base radius.
	Cone&	base( float base ) { mRadiusBase = math<float>::max( base, 0.f ); mCalculationsCached = false; return *this; }
	//! Specifies the apex radius.
	Cone&	apex( float apex ) { mRadiusApex = math<float>::max( apex, 0.f ); mCalculationsCached = false; return *this; }
	//! Specifies the apex radius as a \a ratio of the height. A value of 1.0f yields a cone angle of 45 degrees.
	Cone&	ratio( float ratio ) { mRadiusApex = math<float>::max( mRadiusBase + ratio * mHeight, 0.f ); mCalculationsCached = false; return *this; }
	//! Specifies the base and apex radius separately.
	Cone&	radius( float base, float apex ) { 
		mRadiusBase = math<float>::max(0.f, base); 
		mRadiusApex = math<float>::max(0.f, apex); 
		mCalculationsCached = false; return *this; }
	//!
	virtual Cone&	direction( const vec3 &direction ) override { Cylinder::direction( direction ); return *this; }
	//! Conveniently sets origin, height and direction.
	virtual Cone&	set( const vec3 &from, const vec3 &to ) override { Cylinder::set( from, to ); return *this; }
};

//! Defaults to a plane on the z axis, origin = [0, 0, 0], normal = [0, 1, 0]
class Plane : public Source {
  public:
	//! Defaults to having POSITION, NORMAL, and TEX_COORD_0
	Plane();
	virtual ~Plane() {}

	virtual Plane&	enable( Attrib attrib ) override { mEnabledAttribs.insert( attrib ); mCalculationsCached = false; return *this; }
	virtual Plane&	disable( Attrib attrib ) override { mEnabledAttribs.erase( attrib ); mCalculationsCached = false; return *this; }

	// Specifies the number of times each side is subdivided, ex [2,2] means 4 quads in total. Defaults to [1, 1].
	virtual Plane&	subdivisions( const ivec2 &subdivisions );
	//! Specifies the size in each axis. Defaults to [2, 2], or 1 in each direction
	virtual Plane&	size( const vec2 &size )	{ mSize = size; mCalculationsCached = false; return *this; }
	virtual Plane&	axes( const vec3 &uAxis, const vec3 &vAxis );

	Plane& origin( const vec3 &origin )	{ mOrigin = origin; mCalculationsCached = false; return *this; }
	Plane& normal( const vec3 &normal );

	virtual size_t		getNumVertices() const override		{ calculate(); return mPositions.size(); }
	virtual size_t		getNumIndices() const override		{ calculate(); return mIndices.size(); }
	virtual Primitive	getPrimitive() const override		{ return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;

  protected:
	virtual void	calculate() const;

	ivec2		mSubdivisions;
	vec2		mSize;
	vec3		mOrigin, mAxisU, mAxisV;

	mutable bool						mCalculationsCached;
	mutable std::vector<vec3>			mPositions;
	mutable std::vector<vec2>			mTexCoords;
	mutable std::vector<vec3>			mNormals;
	mutable std::vector<vec3>			mColors;
	mutable std::vector<uint32_t>		mIndices;
};

//////////////////////////////////////////////////////////////////////////////////////
// Modifiers

// By default, attributes pass through the Modifier from the input source -> target
// READ attributes values are captured from mSource, typically to derive other attributes from, and then are passed through
// WRITE attributes prevent the passing of the attribute data from source -> target, to allow the owner of the Modifier to write it later
// READ_WRITE attributes are captured but not passed through to the target
class Modifier : public geom::Target {
  public:
	typedef enum { READ, WRITE, READ_WRITE, IGNORED } Access;

	Modifier( const geom::Source &source, geom::Target *target, const std::map<Attrib,Access> &attribs, Access indicesAccess )
		: mSource( source ), mTarget( target ), mAttribs( attribs ), mIndicesAccess( indicesAccess ), mNumIndices( 0 )
	{}

	virtual uint8_t	getAttribDims( geom::Attrib attr ) const override;
	virtual void copyAttrib( Attrib attr, uint8_t dims, size_t strideBytes, const float *srcData, size_t count ) override;
	virtual void copyIndices( Primitive primitive, const uint32_t *source, size_t numIndices, uint8_t requiredBytesPerIndex ) override;
	
	uint8_t	getReadAttribDims( Attrib attr ) const;
	// not const because consumer is allowed to overwrite this data
	float* getReadAttribData( Attrib attr ) const;

	size_t			getNumIndices() const { return mNumIndices; }
	const uint32_t*	getIndicesData() const { return mIndices.get(); }
		
  protected:
	const geom::Source		&mSource;
	geom::Target			*mTarget;
	
	std::map<Attrib,Access>						mAttribs;
	std::map<Attrib,std::unique_ptr<float[]>>	mAttribData;
	std::map<Attrib,uint8_t>					mAttribDims;
	
	Access									mIndicesAccess;
	std::unique_ptr<uint32_t[]>				mIndices;
	size_t									mNumIndices;
	geom::Primitive							mPrimitive;
};

//! "Bakes" a mat4 transformation into the positions and normals of a geom::Source
class Transform : public Source {
  public:
	//! Does not currently support a projection matrix (i.e. doesn't divide by 'w' )
	Transform( const geom::Source &source, const mat4 &transform )
		: mSource( source ), mTransform( transform )
	{}

	const mat4&			getMatrix() const { return mTransform; }
	void				setMatrix( const mat4 &transform ) { mTransform = transform; }
  
  	virtual size_t		getNumVertices() const override		{ return mSource.getNumVertices(); }
	virtual size_t		getNumIndices() const override		{ return mSource.getNumIndices(); }
	virtual Primitive	getPrimitive() const override		{ return mSource.getPrimitive(); }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;
	
	const geom::Source&		mSource;
	mat4					mTransform;
};

//! Twists a geom::Source around a given axis
class Twist : public Source {
  public:
	Twist( const geom::Source &source )
		: mSource( source ), mAxisStart( 0, -1, 0 ), mAxisEnd( 0, 1, 0 ),
			mStartAngle( (float)-M_PI ), mEndAngle( (float)M_PI )
	{}

	Twist&		axisStart( const vec3 &start ) { mAxisStart = start; return *this; }
	Twist&		axisEnd( const vec3 &end ) { mAxisEnd = end; return *this; }
	Twist&		axis( const vec3 &start, const vec3 &end ) { mAxisStart = start; mAxisEnd = end; return *this; }
	Twist&		startAngle( float radians ) { mStartAngle = radians; return *this; }
	Twist&		endAngle( float radians ) { mEndAngle = radians; return *this; }
  
  	virtual size_t		getNumVertices() const override		{ return mSource.getNumVertices(); }
	virtual size_t		getNumIndices() const override		{ return mSource.getNumIndices(); }
	virtual Primitive	getPrimitive() const override		{ return mSource.getPrimitive(); }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;
	
  protected:
	const geom::Source&		mSource;
	vec3					mAxisStart, mAxisEnd;
	float					mStartAngle, mEndAngle;
};

//! Converts any geom::Source to equivalent vertices connected by lines. Output primitive type is always geom::Primitive::LINES.
class Lines : public Source {
  public:
	Lines( const geom::Source &source )
		: mSource( source )
	{}

	virtual size_t		getNumVertices() const override				{ return mSource.getNumVertices(); }
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override				{ return geom::LINES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override	{ return mSource.getAttribDims( attr ); }
	virtual void		loadInto( Target *target ) const override;
	
  protected:
	const geom::Source&		mSource;
};

//! Modifiers the color of a geom::Source as a function of a 2D or 3D input attribute
class ColorFromAttrib : public Source {
  public:
	ColorFromAttrib( const geom::Source &source, Attrib attrib, const std::function<Colorf(vec2)> &fn )
		: mSource( source ), mAttrib( attrib ), mFnColor2( fn )
	{}
	ColorFromAttrib( const geom::Source &source, Attrib attrib, const std::function<Colorf(vec3)> &fn )
		: mSource( source ), mAttrib( attrib ), mFnColor3( fn )
	{}
	
	Attrib				getAttrib() const { return mAttrib; }
	ColorFromAttrib&	attrib( Attrib attrib ) { mAttrib = attrib; return *this; }

	virtual size_t		getNumVertices() const override				{ return mSource.getNumVertices(); }
	virtual size_t		getNumIndices() const override				{ return mSource.getNumIndices(); }
	virtual Primitive	getPrimitive() const override				{ return mSource.getPrimitive(); }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;
	
  protected:
	const geom::Source&				mSource;
	Attrib							mAttrib;
	std::function<Colorf(vec2)>		mFnColor2;
	std::function<Colorf(vec3)>		mFnColor3;
};

class Extrude : public Source {
  public:
	Extrude( const Shape2d &shape, float distance, float approximationScale = 1.0f );
	
	virtual Extrude&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual Extrude&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }

	//! Sets the distance of extrusion along the axis.
	Extrude&			distance( float dist ) { mDistance = dist; mCalculationsCached = false; return *this; }
	//! Enables or disables front and back caps. Enabled by default.
	Extrude&			caps( bool caps ) { mFrontCap = mBackCap = caps; mCalculationsCached = false; return *this; }
	//! Enables or disables front cap. Enabled by default.
	Extrude&			frontCap( bool cap ) { mFrontCap = cap; mCalculationsCached = false; return *this; }
	//! Enables or disables back cap. Enabled by default.
	Extrude&			backCap( bool cap ) { mBackCap = cap; mCalculationsCached = false; return *this; }
	//! Sets the number of subdivisions along the axis of extrusion
	Extrude&			subdivisions( int sub ) { mSubdivisions = std::max<int>( 1, sub ); mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;
	
  protected:
	void calculate() const;
  
	std::vector<Path2d>				mPaths;
	float							mApproximationScale;
	float							mDistance;
	bool							mFrontCap, mBackCap;
	int								mSubdivisions;
	
	mutable bool							mCalculationsCached;
	mutable std::vector<std::vector<vec2>>	mPathSubdivisionPositions, mPathSubdivisionTangents;
	mutable std::vector<vec3>				mPositions, mNormals, mTexCoords;
	mutable std::vector<uint32_t>			mIndices;
};

class ExtrudeSpline : public Source {
  public:
	ExtrudeSpline( const Shape2d &shape, const ci::BSpline<3,float> &spline, int splineSubdivisions = 10, float approximationScale = 1.0f );
	
	virtual ExtrudeSpline&	enable( Attrib attrib ) { mEnabledAttribs.insert( attrib ); return *this; }
	virtual ExtrudeSpline&	disable( Attrib attrib ) { mEnabledAttribs.erase( attrib ); return *this; }

	//! Enables or disables front and back caps. Enabled by default.
	ExtrudeSpline&		caps( bool caps ) { mFrontCap = mBackCap = caps; mCalculationsCached = false; return *this; }
	//! Enables or disables front cap. Enabled by default.
	ExtrudeSpline&		frontCap( bool cap ) { mFrontCap = cap; mCalculationsCached = false; return *this; }
	//! Enables or disables back cap. Enabled by default.
	ExtrudeSpline&		backCap( bool cap ) { mBackCap = cap; mCalculationsCached = false; return *this; }
	//! Sets the number of subdivisions along the axis of extrusion
	ExtrudeSpline&		subdivisions( int sub ) { mSubdivisions = std::max<int>( 1, sub ); mCalculationsCached = false; return *this; }

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override;
	virtual Primitive	getPrimitive() const override { return Primitive::TRIANGLES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;
	
  protected:
	void calculate() const;
  
	std::vector<Path2d>				mPaths;
	std::vector<mat4>				mSplineFrames;
	std::vector<float>				mSplineTimes;
	float							mApproximationScale;
	bool							mFrontCap, mBackCap;
	int								mSubdivisions;
	
	mutable bool							mCalculationsCached;
	mutable std::vector<std::vector<vec2>>	mPathSubdivisionPositions, mPathSubdivisionTangents;
	mutable std::vector<vec3>				mPositions, mNormals, mTexCoords;
	mutable std::vector<uint32_t>			mIndices;
};

//! Draws lines representing the Attrib::NORMALs for a geom::Source. Encodes 0 for base and 1 for normal into CUSTOM_0. Prevents pass-through of NORMAL and COLOR. Passes CI_TEX_COORD_0
class VertexNormalLines : public Source {
  public:
	VertexNormalLines( const geom::Source &source, float length )
		: mSource( source ), mLength( length )
	{}

	VertexNormalLines&	length( float len ) { mLength = len; return *this; }

	virtual size_t		getNumVertices() const override;
	virtual size_t		getNumIndices() const override				{ return 0; }
	virtual Primitive	getPrimitive() const override				{ return geom::LINES; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;
	
  protected:
	const geom::Source&		mSource;
	float					mLength;
};

class BSpline : public Source {
  public:
	template<int D, typename T>
	BSpline( const ci::BSpline<D,T> &spline, int subdivisions );

	virtual size_t		getNumVertices() const override				{ return mNumVertices; }
	virtual size_t		getNumIndices() const override				{ return 0; }
	virtual Primitive	getPrimitive() const override				{ return geom::LINE_STRIP; }
	virtual uint8_t		getAttribDims( Attrib attr ) const override;
	virtual void		loadInto( Target *target ) const override;
	
  protected:
	template<typename T>
	void init( const ci::BSpline<2,T> &spline, int subdivisions );
	template<typename T>
	void init( const ci::BSpline<3,T> &spline, int subdivisions );
	template<typename T>
	void init( const ci::BSpline<4,T> &spline, int subdivisions );

	int						mPositionDims;
	size_t					mNumVertices;
	std::vector<float>		mPositions;
	std::vector<vec3>		mNormals;
};

class Exc : public Exception {
};

class ExcMissingAttrib : public Exception {
};

class ExcIllegalSourceDimensions : public Exception {
};

class ExcIllegalDestDimensions : public Exception {
};

class ExcIllegalPrimitiveType : public Exception {
};

class ExcNoIndices : public Exception {
};

class ExcIllegalIndexType : public Exception {
};

// Attempt to store >65535 indices into a uint16_t
class ExcInadequateIndexStorage : public Exception {
};

} } // namespace cinder::geom
