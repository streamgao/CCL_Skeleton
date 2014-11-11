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
#include "LightSource.h"
#include "ShadowMap.h"

using namespace ci;
using namespace ci::app;


class StreamGaoApp : public AppNative {
public:
    void prepareSettings( AppBasic::Settings *settings ) override;
    void	setup() override;
    void	resize() override;
    void	update() override;
    void	draw() override;
    
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    
    void keyDown( KeyEvent event )override;
    
    void renderShadowMap();
    
    CameraPersp			mCam;
    MayaCamUI			mMayaCam;
    
    gl::BatchRef		mBatchSimple;
    gl::BatchRef		mBatch;
    gl::BatchRef        mPlane;
    
    gl::GlslProgRef		mGlsl;
    mat4				mCubeRotation;
    
    
private:
    ShadowMapRef        mShadowMap;
    int                 mShadowMapSize;
    bool                mOnlyShadowap;
    
    
    LightSource         mLightSource;
    
    int                 mShadowTechnique;
    
    float						mDepthBias;
    bool						mEnableNormSlopeOffset;
    float						mRandomOffset;
    int							mNumRandomSamples;
    float						mPolygonOffsetFactor, mPolygonOffsetUnits;
};

void StreamGaoApp::prepareSettings( AppBasic::Settings *settings )
{
    settings->setWindowSize( 600, 400 );
}

void StreamGaoApp::setup()
{
    mCam.setEyePoint( vec3( 0, 0, 25 ) );
    mCam.setCenterOfInterestPoint( vec3( 0 ) );
    
    mLightSource.setPosition( vec3( 0, 25, 1 ) );
    mLightSource.lookAt( vec3( 0, 0, 0 ) );
    
    // Load and compile the shader.
    mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
    
    // Load the clover and attach our shader to it.
    //auto clover = ObjLoader( loadAsset( "clover.obj" ) );
    auto clover = geom::Sphere().subdivisions( 30 );
    mBatch = gl::Batch::create( clover, mGlsl );
    mPlane = gl::Batch::create( geom::Plane().subdivisions( ivec2( 10, 10 ) ).size( vec2( 1000, 1000 ) ).axes( vec3( 1, 0, 0 ), vec3( 0, 0, 1 ) ), mGlsl );
    
    // Also create a version using a very basic shader, which we can use when rendering the shadow map.
    // Cinder can create a standard shader for us, using 'getStockShader()'.
    mBatchSimple = gl::Batch::create( clover, gl::context()->getStockShader( gl::ShaderDef() ) );
    
    // Create a nice, big shadow map (texture).
    mShadowMap = ShadowMap::create( 4096 );
    
    gl::enableDepthWrite();
    gl::enableDepthRead();
}

void StreamGaoApp::resize()
{
    mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
    gl::setMatrices( mCam );
}

void StreamGaoApp::update()
{
    float time = (float) getElapsedSeconds();
    
    // Rotate the cube around an arbitrary axis
    mCubeRotation = mat4();
    mCubeRotation *= rotate( time, normalize( vec3( 0, 1, 0.5 ) ) );
    mCubeRotation *= scale( vec3( 10 ) );
    
}

void StreamGaoApp::draw()
{
//    gl::clear();
//    
//    // Render the scene as seen from the light. Instead of drawing to our window,
//    // we'll draw it to a texture so we can use it as a shadow map later on.
//    renderShadowMap();
//    
//    // Now that we have our shadow map, let's render the whole scene to the window.
//    gl::setMatrices( mCam );
//    
//    auto shader = mBatch->getGlslProg();
//    
//    shader->uniform( "Ka", vec3( 1.0, 0.0, 0.0 ) );
//    shader->uniform( "Kd", vec3( 1.0, 1.0, 0.0 ) );
//    shader->uniform( "Ks", vec3( 1.0, 1.0, 1.0 ) );
//    shader->uniform( "Shininess", 128.0f );
//    
//    shader->uniform( "uShadowMap", 0 ); // Use the texture that is bound to unit 0.
//    shader->uniform( "uShadowMatrix", mLightSource.getShadowMatrix() );
//    
//    shader->uniform( "uLightPosition", vec4( mLightSource.getPosition(), 1 ) );
//    shader->uniform( "uLightIntensity", vec3( 1.0, 1.0, 1.0 ) );
//    shader->uniform( "uLightViewMatrix", mLightSource.getViewMatrix() );
//    shader->uniform( "uLightProjectionMatrix", mLightSource.getProjectionMatrix() );
//    
//    // Bind the shadow map to texture unit 0.
//    gl::ScopedTextureBind tex0( mShadowMap->getTexture(), (uint8_t) 0 );
//    
//    gl::pushMatrices();
//    gl::multModelMatrix( mCubeRotation );
//    mBatch->draw();
//    gl::popMatrices();
    //*/
}

void StreamGaoApp::renderShadowMap()
{
//    // Don't render to the window, but render to our texture instead:
//    gl::ScopedFramebuffer fbo( mShadowMap->getFbo() );
//    
//    gl::ScopedViewport viewport( ivec2( 0 ), mShadowMap->getSize() );
//    
//    gl::clear();
//    
//    // Render the scene as seen from the light source.
//    mLightSource.setMatrices();
//    
//    // Offset to help combat surface acne (self-shadowing)
//    const float kPolygonOffsetFactor = 1.0f;
//    const float kPolygonOffsetUnits = 1.0f;
//    
//    gl::enable( GL_POLYGON_OFFSET_FILL );
//    glPolygonOffset( kPolygonOffsetFactor, kPolygonOffsetUnits );
//    
//    // Render the scene!
//    gl::pushMatrices();
//    gl::multModelMatrix( mCubeRotation );
//    mBatchSimple->draw();
//    gl::popMatrices();
//    
//    gl::disable( GL_POLYGON_OFFSET_FILL );
}

void StreamGaoApp::mouseDown( MouseEvent event )
{
    mMayaCam.setCurrentCam( mCam );
    mMayaCam.mouseDown( event.getPos() );
}

void StreamGaoApp::mouseDrag( MouseEvent event )
{
    mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
    mCam = mMayaCam.getCamera();
}

void StreamGaoApp::keyDown( KeyEvent event )
{
    switch( event.getCode() ) {
        case KeyEvent::KEY_SPACE:
            mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
            mBatch->setGlslProg( mGlsl );
            break;
    }
}

CINDER_APP_NATIVE( StreamGaoApp, RendererGl )









