#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/GeomIo.h"
#include "cinder/Rand.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Log.h"
#include "cinder/Color.h"

#include "Bloom.h"

using namespace ci;
using namespace ci::app;



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
        gl::Texture2d::Format depthFormat;
        depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
        depthFormat.setMagFilter( GL_LINEAR );
        depthFormat.setMinFilter( GL_LINEAR );
        depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
        depthFormat.setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
        depthFormat.setCompareFunc( GL_LEQUAL );
        mTextureShadowMap = gl::Texture2d::create( size, size, depthFormat );
        
        gl::Fbo::Format fboFormat;
        fboFormat.attachment( GL_DEPTH_ATTACHMENT, mTextureShadowMap );
        mShadowMap = gl::Fbo::create( size, size, fboFormat );
    }
    
    const gl::FboRef&		getFbo() const { return mShadowMap; }
    const gl::Texture2dRef&	getTexture() const { return mTextureShadowMap; }
    
    float					getAspectRatio() const { return mShadowMap->getAspectRatio(); }
    ivec2					getSize() const { return mShadowMap->getSize(); }
private:
    gl::FboRef				mShadowMap;
    gl::Texture2dRef		mTextureShadowMap;
};

struct LightData {
    bool						toggleViewpoint;
    float						distanceRadius;
    float						fov;
    CameraPersp					camera;
    vec3						viewpoint;
    vec3						target;
};



class RotatingCubeApp : public AppNative {
  public:
    void prepareSettings( AppBasic::Settings *settings ) override;
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	gl::BatchRef		mBatch;
	gl::TextureRef		mTexture;
	gl::GlslProgRef		mGlsl;
	mat4				mCubeRotation;
    //include
    Bloom               bl;
    
  private:
    ShadowMapRef        mShadowMap;
    int                 mShadowMapSize;
    bool                mOnlyShadowap;
    
    LightData           mLight;
    
    int                 mShadowTechnique;
    
    float						mDepthBias;
    bool						mEnableNormSlopeOffset;
    float						mRandomOffset;
    int							mNumRandomSamples;
    float						mPolygonOffsetFactor, mPolygonOffsetUnits;
};

void RotatingCubeApp::prepareSettings(AppBasic::Settings *settings){
    settings->setWindowSize(600, 400);
}

void RotatingCubeApp::setup()
{
	mCam.lookAt( vec3( 0, 0, 25 ), vec3( 0 ) );
	
	//mTexture = gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ), gl::Texture::Format().mipmap() );
    //bl.resize( vec2(1,1) );

//#if defined( CINDER_GL_ES )
//	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
//#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
//#endif
	mBatch = gl::Batch::create( ObjLoader( loadAsset( "clover.obj" ) ), mGlsl );

    
	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void RotatingCubeApp::resize()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	// Rotate the cube by 2 degrees around an arbitrary axis
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 0, 1, 0.5 ) ) );

}

void RotatingCubeApp::draw()
{
	gl::clear( );

	gl::setMatrices( mCam );
	//mTexture->bind();
    auto shader = mBatch->getGlslProg();
    
    shader->uniform( "LightPosition", vec4( 0, 0, 25, 1 ) );
    shader->uniform( "LightIntensity", vec3( 1.0, 1.0, 1.0 ) );
    shader->uniform( "Kd", vec3( 0, 0, 0) );
    shader->uniform( "Ka", vec3( 0.22, 0.20725, 0.20725) );
    shader->uniform( "Ks", vec3(0.596648, 0.596648, 0 ) );
    shader->uniform( "Shininess", 500.0f );
    
    //bl.bloom(mTexture);
    
	gl::pushMatrices();
		gl::multModelMatrix( mCubeRotation );
		mBatch->draw();
	gl::popMatrices();
}

CINDER_APP_NATIVE( RotatingCubeApp, RendererGl )









