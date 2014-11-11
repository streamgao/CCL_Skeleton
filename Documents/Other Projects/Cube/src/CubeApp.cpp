#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/ObjLoader.h"

using namespace ci;
using namespace ci::app;

class RotatingCubeApp : public AppNative {
  public:	
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	
	CameraPersp			mCam;
	gl::BatchRef		mBatch;
	gl::TextureRef		mTexture;
	gl::GlslProgRef		mGlsl;
	mat4				mCubeRotation;
};

void RotatingCubeApp::setup()
{
	mCam.lookAt( vec3( 0, 0, 25 ), vec3( 0 ) );
	
	mTexture = gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ), gl::Texture::Format().mipmap() );

#if defined( CINDER_GL_ES )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif
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
	mCubeRotation *= rotate( toRadians( 0.2f ), normalize( vec3( 1, 1, 0 ) ) );
}

void RotatingCubeApp::draw()
{
	gl::clear();

	gl::setMatrices( mCam );
	mTexture->bind();
    auto shader = mBatch->getGlslProg();
    
    shader->uniform( "LightPosition", vec4( 0, 0, 25, 1 ) );
    shader->uniform( "LightIntensity", vec3( 1.0, 1.0, 1.0 ) );
    shader->uniform( "Kd", vec3( .7, 0.0, 0.0 ) );
    shader->uniform( "Ka", vec3( 0.2, 0.2, 0.2 ) );
    shader->uniform( "Ks", vec3( 1.0, 1.0, 1.0 ) );
    shader->uniform( "Shininess", 128.0f );
    
	gl::pushMatrices();
		gl::multModelMatrix( mCubeRotation );
		mBatch->draw();
	gl::popMatrices();
}

CINDER_APP_NATIVE( RotatingCubeApp, RendererGl )