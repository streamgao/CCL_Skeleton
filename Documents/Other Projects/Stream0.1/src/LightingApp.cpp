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

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ObjLoader.h"
#include "cinder/Timeline.h"
#include "cinder/TriMesh.h"
#include "cinder/Utilities.h"

#include "Bloom.h"
#include "LightSource.h"
#include "ShadowMap.h"
#include "smaa/SMAA.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LightingApp : public AppNative {
public:
	void prepareSettings( Settings* settings ) override;

	void setup() override;
	void update() override;
	void draw() override;

	void mouseMove( MouseEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;

	void keyDown( KeyEvent event ) override;

	void resize() override;
	void fileDrop( FileDropEvent event ) override;

	void loadShaders();
	void loadMesh( DataSourceRef source );

	void showTitle( const std::string& title );
	void hideTitle();
	void drawTitle();

private:
	gl::GlslProgRef         mShader;
	gl::GlslProgRef         mShaderDepth;
	gl::GlslProgRef         mShaderPostProcessing;

	gl::FboRef              mFboScene;
	gl::FboRef              mFboFinal;

	gl::BatchRef            mBatch;
	float                   mBatchScale;

	ShadowMapRef            mShadowMap;

	Bloom                   mBloom;
	//SMAA                    mSMAA;

	CameraPersp             mCamera;
	MayaCamUI               mMayaCam;

	LightSource             mLight;
	Anim<Color>             mLightColor;
	Anim<float>             mLightIntensity;

	Anim<float>             mAmbientStrength;
	vec3                    mAmbientSkyDirection;
	Anim<Color>             mAmbientSkyColor;
	Anim<Color>             mAmbientGroundColor;

	Anim<float>             mDiffuseStrength;

	Anim<float>             mSpecularStrength;
	Anim<float>             mSpecularShininess;

	Anim<float>             mRimStrength;
	Anim<float>             mTranslucentStrength;

	Anim<float>             mShadowStrength;

	Anim<float>             mOriginalStrength;
	Anim<float>             mBloomStrength;

	Font                    mTitleFont;
	gl::TextureFontRef      mTitle;
	std::string             mTitleText;
	std::string             mTitleNext;
	Anim<float>             mTitleAlpha;

	PausableTimer           mTimeModel;
	PausableTimer           mTimeLight;

	double                  mTime;
	double                  mTimeCursor;

	bool                    mShowTitle;
};

void LightingApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 1440, 900 );
	settings->setFrameRate( 60 );
	//settings->setFullScreen();
}

void LightingApp::setup()
{
	// Load assets.
	loadShaders();
#if USE_RESOURCES
	loadMesh( loadResource( RES_MESH_VENUS ) );
#else
	loadMesh( loadFile( getAssetPath( "venus.msh" ) ) );
#endif

	// Initialize titles.
#if USE_RESOURCES
	mTitleFont = Font( loadResource( RES_FONT_DOSIS_LT ), 48 );
#else
	mTitleFont = Font( loadAsset( "fonts/Dosis-Light.ttf" ), 48 );
#endif
	mTitle = gl::TextureFont::create( mTitleFont, gl::TextureFont::Format().premultiply( true ) );

	mShowTitle = false;

	// Initialize bloom and SMAA
	mBloom.setup();
	//mSMAA.setup();

	// Initialize animation timer.
	timeline().add( [&]( void ) {mTimeModel.start(); }, timeline().getCurrentTime() + 2.5f );

	// Initialize parameters.
	mAmbientStrength = 0.1f;
	mAmbientSkyDirection = vec3( 0, 1, 0 );
	mAmbientSkyColor = Color( 0.2f, 0.6f, 1.0f );
	mAmbientGroundColor = Color( 0, 0, 0 );
	mDiffuseStrength = 0.0f;
	mSpecularStrength = 1.0f;
	mSpecularShininess = 2.0f;
	mRimStrength = 0.5f;
	mTranslucentStrength = 1.0f;
	mShadowStrength = 1.0f;

	mOriginalStrength = 0.0f;

	mBloomStrength = 0.5f;
	mBloom.setAfterImage( 0.95f );

	// Fade-in at start.
	timeline().apply( &mOriginalStrength, 0.0f, 1.0f, 5.0f ).delay( 2.5f );

	// Animate light source after an initial delay.
	timeline().add( [&]() { mTimeLight.resume(); }, 16.0f );

	// Setup camera and light source.
	mCamera.setEyePoint( vec3( -3.8f, 3.6f, 0.4f ) );
	mCamera.setCenterOfInterestPoint( vec3( 0.0f, 2.6f, 0.0f ) );

	mLightIntensity = 1.5f;
	mLightColor = Color( 1.0f, 0.9f, 0.8f );
	mLight.setColor( mLightColor );
	mLight.setRange( 150.0f );
	mLight.setPosition( vec3( 2.1f, 2.85f, -1.9f ) );
	mLight.lookAt( mCamera.getCenterOfInterestPoint() );

	mTime = 0.0;
	mTimeCursor = getElapsedSeconds();
}

void LightingApp::update()
{
	// Advance time if not paused.
	double elapsed = getElapsedSeconds() - mTime;
	mTime += elapsed;

	// Show frame rate in window title.
	std::string title( "Lighting Demo" );
	title += " - Frame rate: " + toString( elapsed > 0.0 ? int( 1.0 / elapsed ) : 1000 );
	getWindow()->setTitle( title );

	// Update light position and color.
	float t = 0.125f * (float) mTimeLight.getSeconds();
	float x = 3.0f * math<float>::cos( t );
	float y = 2.0f + 1.0f * math<float>::sin( t * 0.25f );
	float z = 3.0f * math<float>::sin( t );

	mLight.setPosition( vec3( x, y, z ) );
	mLight.setColor( mLightColor );

	if( getElapsedSeconds() - mTimeCursor > 3.0 )
		hideCursor();
	else
		showCursor();
}

void LightingApp::draw()
{
	// Clear the main buffer.
	gl::clear();

	// Enable depth buffer.
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// Make sure diffuse and specular respect the energy conservation rules.
	float energy = 1.0f / math<float>::max( 1.0f, mDiffuseStrength() + mSpecularStrength() );

	// Get the model rotation.
	float rotation = 0.1f * (float) mTimeModel.getSeconds();

	// Render depth pass for subsurface scattering effect.
	if( mShadowMap ) {
		const int w = mShadowMap->getFbo()->getWidth();
		const int h = mShadowMap->getFbo()->getHeight();

		gl::ScopedFramebuffer fbo( mShadowMap->getFbo() );
		gl::ScopedViewport viewport( 0, 0, w, h );
		gl::clear();

		// Enable depth buffer.
		gl::enableDepthRead();
		gl::enableDepthWrite();

		gl::enable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 6.0f, 3.0f );

		gl::pushMatrices();
		mLight.setMatrices();

		if( mBatch ) {
			gl::pushModelMatrix();
			gl::rotate( rotation, vec3( 0, 1, 0 ) );
			gl::scale( mBatchScale, mBatchScale, mBatchScale );
			gl::enable( GL_CULL_FACE );
			glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

			mBatch->setGlslProg( mShaderDepth );
			mBatch->draw();

			glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
			gl::popModelMatrix();
		}

		gl::popMatrices();

		gl::disable( GL_POLYGON_OFFSET_FILL );
	}

	// Render scene.
	if( mFboScene && mShadowMap ) {
		gl::ScopedFramebuffer fbo( mFboScene );
		gl::ScopedTextureBind tex0( mShadowMap->getTexture() );

		gl::clear();
		gl::setMatrices( mCamera );

		{
			gl::ScopedGlslProg shader( mShader );
			mShader->uniform( "uShadowMap", 0 );
			mShader->uniform( "uShadowMatrix", mLight.getShadowMatrix() );
			mShader->uniform( "uLightViewMatrix", mLight.getViewMatrix() );
			mShader->uniform( "uLightColor", mLight.getColor() * mLightIntensity() );
			mShader->uniform( "uLightViewPosition", gl::getViewMatrix() * vec4( mLight.getPosition(), 1 ) );
			mShader->uniform( "uLightViewDirection", gl::getViewMatrix() * vec4( mLight.getDirection(), 0 ) );
			mShader->uniform( "uLightParams", vec4( 1.0f / mLight.getRange(), 0, 0, 0 ) );

			mShader->uniform( "uAmbientStrength", mAmbientStrength() );
			mShader->uniform( "uAmbientSkyColor", mAmbientSkyColor() );
			mShader->uniform( "uAmbientGroundColor", mAmbientGroundColor() );
			mShader->uniform( "uSkyViewDirection", gl::getViewMatrix() * vec4( 0, 1, 0, 0 ) );

			mShader->uniform( "uDiffuseStrength", mDiffuseStrength() * energy );
			mShader->uniform( "uSpecularStrength", mSpecularStrength() * energy );
			mShader->uniform( "uSpecularShininess", math<float>::pow( 10.0f, mSpecularShininess ) );
			mShader->uniform( "uRimStrength", mRimStrength() );
			mShader->uniform( "uTranslucentStrength", mTranslucentStrength() );

			mShader->uniform( "uShadowStrength", mShadowStrength );

			if( mBatch ) {
				gl::pushModelMatrix();
				gl::rotate( rotation, vec3( 0, 1, 0 ) );
				gl::scale( mBatchScale, mBatchScale, mBatchScale );
				gl::enable( GL_CULL_FACE );
				mBatch->setGlslProg( mShader );
				mBatch->draw();
				gl::popModelMatrix();
			}
		}

		// Render light source.
		gl::disableDepthWrite();
		gl::color( mLight.getColor() * mLightIntensity );
		gl::drawSphere( mLight.getPosition(), 0.05f, 36 );
		gl::enableDepthWrite();
	}

	// Initialize 2D rendering.
	gl::setMatricesWindow( getWindowSize() );
	gl::disableDepthRead();
	gl::disableDepthWrite();

	// Apply bloom effect and filmic tone mapping, then blit to final buffer.
	{
		gl::Texture2dRef bloom = mBloom.bloom( mFboScene->getColorTexture() );

		gl::ScopedTextureBind tex0( mFboScene->getColorTexture(), (uint8_t) 0 );
		gl::ScopedTextureBind tex1( bloom, (uint8_t) 1 );

		gl::ScopedGlslProg shader( mShaderPostProcessing );
		mShaderPostProcessing->uniform( "uTexScene", 0 );
		mShaderPostProcessing->uniform( "uTexBloom", 1 );
		mShaderPostProcessing->uniform( "uColorScene", ColorA( mOriginalStrength, mOriginalStrength, mOriginalStrength, 1 ) );
		mShaderPostProcessing->uniform( "uColorBloom", ColorA( mBloomStrength, mBloomStrength, mBloomStrength, 1 ) );

		//gl::ScopedFramebuffer fbo( mFboFinal );

		gl::color( 1, 1, 1 );
		gl::drawSolidRect( getWindowBounds() );
	}

	// Apply post processing anti aliasing.
	//mSMAA.draw( mFboFinal->getColorTexture(), getWindowBounds() );

	//gl::color( 1, 1, 1 );
	//gl::draw( mSMAA.getEdgePass() );

	// Draw title.
	drawTitle();
}

void LightingApp::mouseMove( MouseEvent event )
{
	mTimeCursor = getElapsedSeconds();
}

void LightingApp::mouseDown( MouseEvent event )
{
	mTimeCursor = getElapsedSeconds();

	// Handle camera user input.
	mMayaCam.setCurrentCam( mCamera );
	mMayaCam.mouseDown( event.getPos() );
}

void LightingApp::mouseDrag( MouseEvent event )
{
	mTimeCursor = getElapsedSeconds();

	// Handle camera user input.
	mMayaCam.mouseDrag( event.getPos(),
						!event.isShiftDown() && !event.isAccelDown(),
						event.isShiftDown() && !event.isAccelDown(),
						event.isAccelDown() && !event.isShiftDown() );
	mCamera = mMayaCam.getCamera();
}

void LightingApp::keyDown( KeyEvent event )
{
	// Handle keyboard user inut.
	switch( event.getCode() ) {
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_SPACE:
		if( mTimeModel.isPaused() )
			mTimeModel.resume();
		else
			mTimeModel.pause();
		break;
	case KeyEvent::KEY_v:
		gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
		break;
		//case KeyEvent::KEY_f:
		//	setFullScreen( !isFullScreen() );
		//	break;
	case KeyEvent::KEY_RETURN:
		loadShaders();
		break;
	case KeyEvent::KEY_RIGHTBRACKET:
		if( mShadowMap && mShadowMap->getSize().x < 4096 )
			mShadowMap->reset( mShadowMap->getSize().x << 1 );
		break;
	case KeyEvent::KEY_LEFTBRACKET:
		if( mShadowMap && mShadowMap->getSize().x > 2 )
			mShadowMap->reset( mShadowMap->getSize().x >> 1 );
		break;
	case KeyEvent::KEY_EQUALS:
		// default
		timeline().apply( &mAmbientStrength, 0.1f, 1.0f );
		timeline().apply( &mDiffuseStrength, 0.0f, 1.0f ).delay( 1.0f );
		timeline().apply( &mSpecularStrength, 1.0f, 1.0f ).delay( 2.0f );
		timeline().apply( &mRimStrength, 1.0f, 1.0f ).delay( 3.0f );
		timeline().apply( &mTranslucentStrength, 1.0f, 1.0f ).delay( 4.0f );
		timeline().apply( &mShadowStrength, 1.0f, 0.1f );
		timeline().apply( &mBloomStrength, mBloom.getAfterImage() > 0.0f ? 0.5f : 1.0f, 0.1f );
		hideTitle();
		break;
	case KeyEvent::KEY_0:
		// all off
		timeline().apply( &mAmbientStrength, 0.0f, 1.0f );
		timeline().apply( &mDiffuseStrength, 0.0f, 1.0f );
		timeline().apply( &mSpecularStrength, 0.0f, 1.0f );
		timeline().apply( &mRimStrength, 0.0f, 1.0f );
		timeline().apply( &mTranslucentStrength, 0.0f, 1.0f );
		//timeline().apply( &mBloomStrength, 0.0f, 1.0f );
		//timeline().add( [&]() { mTimeLight.pause( 0.0 ); }, timeline().getCurrentTime() + 1.0f );
		//timeline().add( [&]() { mTimeModel.pause( 0.0 ); }, timeline().getCurrentTime() + 1.0f );
		hideTitle();
		mShowTitle = true;
		break;
	case KeyEvent::KEY_1:
		// toggle ambient
		if( event.isShiftDown() )
			timeline().apply( &mAmbientStrength, 0.0f, 1.0f );
		else if( event.isControlDown() ) {
			timeline().apply( &mAmbientGroundColor, mAmbientSkyColor(), 1.0f );
			showTitle( "Solid Color Ambient Lighting" );
			mShowTitle = true;
		}
		else {
			timeline().apply( &mAmbientStrength, 0.1f, 1.0f );
			timeline().apply( &mAmbientGroundColor, Color( 0, 0, 0 ), 1.0f );
			if( mShowTitle )
				showTitle( "Hemispherical Ambient Lighting" );
			else
				hideTitle();
		}
		break;
	case KeyEvent::KEY_2:
		// toggle diffuse
		if( event.isShiftDown() )
			timeline().apply( &mDiffuseStrength, 0.0f, 1.0f );
		else {
			timeline().apply( &mDiffuseStrength, 1.0f, 1.0f );
			if( mShowTitle ) {
				showTitle( "Diffuse Lighting" );

				// For demo purposes.
				mTimeLight.pause( 17.0 );
				mTimeModel.pause( 0.0 );
				mCamera.setEyePoint( vec3( -3.8f, 3.6f, 0.4f ) );
				mCamera.setCenterOfInterestPoint( vec3( 0.0f, 2.6f, 0.0f ) );
				mBloomStrength = 0.0f;
			}
			else
				hideTitle();
		}
		break;
	case KeyEvent::KEY_3:
		// toggle specular
		if( event.isShiftDown() )
			timeline().apply( &mSpecularStrength, 0.0f, 1.0f );
		else {
			timeline().apply( &mSpecularStrength, 1.0f, 1.0f );
			if( mShowTitle ) {
				showTitle( "Specular Lighting" );

				// For demo purposes.
				mTimeLight.pause( 13.0 );
				mTimeModel.start( 0.0 );
				mCamera.setEyePoint( vec3( -3.8f, 3.6f, 0.4f ) );
				mCamera.setCenterOfInterestPoint( vec3( 0.0f, 2.6f, 0.0f ) );
				mBloomStrength = 0.0f;
			}
			else
				hideTitle();
		}
		break;
	case KeyEvent::KEY_4:
		// toggle rim
		if( event.isShiftDown() )
			timeline().apply( &mRimStrength, 0.0f, 1.0f );
		else {
			timeline().apply( &mRimStrength, 0.5f, 1.0f );
			if( mShowTitle )
				showTitle( "Rim Lighting" );
			else
				hideTitle();
		}
		break;
	case KeyEvent::KEY_5:
		// toggle absorption
		if( event.isShiftDown() )
			timeline().apply( &mTranslucentStrength, 0.0f, 1.0f );
		else {
			timeline().apply( &mTranslucentStrength, 1.0f, 1.0f );
			if( mShowTitle ) {
				showTitle( "Light Absorption" );

				// For demo purposes.
				mTimeLight.pause( 2.0 );
				mCamera.setEyePoint( vec3( -3.8f, 3.6f, 0.4f ) );
				mCamera.setCenterOfInterestPoint( vec3( 0.0f, 2.6f, 0.0f ) );
				mBloom.setAfterImage( 0.95f );
				timeline().apply( &mBloomStrength, 0.5f, 1.0f );
			}
			else
				hideTitle();
		}
		break;
	case KeyEvent::KEY_m:
		// shadow strength
		if( event.isShiftDown() ) {
			timeline().apply( &mShadowStrength, 0.0f, 1.0f );
			showTitle( "Shadows Disabled" );
		}
		else {
			timeline().apply( &mShadowStrength, 1.0f, 1.0f );
			showTitle( "Shadows Enabled" );
		}
		break;
	case KeyEvent::KEY_s:
		// change shininess
		if( event.isShiftDown() )
			timeline().apply( &mSpecularShininess, 1.4f, 2.0f );
		else
			timeline().apply( &mSpecularShininess, 3.0f, 2.0f );
		break;
	case KeyEvent::KEY_h:
		// change hemi-ambient colors
		if( event.isShiftDown() ) {
			timeline().apply( &mAmbientGroundColor, Color( 1, 0, 0 ), 1.0f ).delay( 1.0f );
			timeline().apply( &mAmbientSkyColor, Color( 0, 0, 0 ), 1.0f );
			timeline().apply( &mAmbientStrength, 1.0f, 1.0f ).delay( 1.0f );
		}
		else {
			timeline().apply( &mAmbientGroundColor, Color( 0, 0, 0 ), 1.0f );
			timeline().apply( &mAmbientSkyColor, Color( 0.2f, 0.6f, 1.0f ), 1.0f ).delay( 1.0f );
			timeline().apply( &mAmbientStrength, 0.1f, 1.0f );
		}
		break;
	case KeyEvent::KEY_b:
		// blooming
		if( event.isControlDown() ) {
			timeline().apply( &mOriginalStrength, 0.0f, 1.0f );
			timeline().apply( &mBloomStrength, 1.0f, 1.0f );
			showTitle( "Blooming Only" );
		}
		else if( event.isShiftDown() ) {
			timeline().apply( &mOriginalStrength, 1.0f, 1.0f );
			timeline().apply( &mBloomStrength, 0.0f, 1.0f );
			showTitle( "Blooming Disabled" );
		}
		else {
			timeline().apply( &mOriginalStrength, 1.0f, 1.0f );
			timeline().apply( &mBloomStrength, mBloom.getAfterImage() > 0.0f ? 0.5f : 1.0f, 1.0f );
			showTitle( "Blooming Enabled" );
		}
		break;
	case KeyEvent::KEY_a:
		// bloom after image
		if( event.isShiftDown() ) {
			mBloom.setAfterImage( 0.95f );
			timeline().apply( &mBloomStrength, 0.5f, 1.0f );
			//showTitle( "Temporal Blooming" );
		}
		else {
			mBloom.setAfterImage( 0.0f );
			timeline().apply( &mBloomStrength, 1.0f, 1.0f );
			hideTitle();
		}
		break;
	case KeyEvent::KEY_l:
		// animate light
		if( mTimeLight.isPaused() )
			mTimeLight.resume();
		else
			mTimeLight.pause();
		break;
	case KeyEvent::KEY_x:
		/*// light intensity
		if( event.isShiftDown() ) {
			timeline().apply( &mLightIntensity, 2.0f, 1.0f );
			showTitle( "Extra Bright Light" );
		}
		else {
			timeline().apply( &mLightIntensity, 1.5f, 1.0f );
			showTitle( "Normal Light Intensity" );
		}*/
		break;
	case KeyEvent::KEY_c:
		// light color
		if( event.isShiftDown() )
			timeline().apply( &mLightColor, Color( 0, 1, 0 ), 2.0f );
		else
			timeline().apply( &mLightColor, Color( 1.0f, 0.9f, 0.8f ), 2.0f );
		break;
	}
}

void LightingApp::resize()
{
	// Resize our camera.
	mCamera.setAspectRatio( getWindowAspectRatio() );

	// Resize bloom.
	mBloom.resize( getWindowSize() );

	// Create frame buffers if necessary.
	const int w = getWindowWidth();
	const int h = getWindowHeight();

	try {
		gl::Fbo::Format fmt;
		fmt.colorTexture( gl::Texture2d::Format().internalFormat( GL_RGB16F )
						  .mipmap( false ).minFilter( GL_LINEAR ).magFilter( GL_LINEAR ) );
		fmt.setSamples( 8 );

		mFboScene = gl::Fbo::create( w, h, fmt );

		fmt.colorTexture( gl::Texture2d::Format().internalFormat( GL_RGBA8 ).minFilter( GL_NEAREST ).magFilter( GL_NEAREST ) );

		mFboFinal = gl::Fbo::create( w, h, fmt );
	}
	catch( const std::exception& e ) {
		console() << e.what() << std::endl;
		quit();
	}

	if( !mShadowMap )
		mShadowMap = ShadowMap::create( 2048 );
}

void LightingApp::fileDrop( FileDropEvent event )
{
#if !USE_RESOURCES
	// Load dropped file (supported are ".obj" and ".msh" files).
	fs::path file = event.getFile( 0 );

	if( file.extension() == ".obj" ) {
		ObjLoader loader( loadFile( file ) );
		TriMesh mesh( loader );

		file.replace_extension( ".msh" );
		if( !fs::exists( file ) )
			mesh.write( writeFile( file ) );
	}

	if( file.extension() == ".msh" ) {
		loadMesh( loadFile( file ) );
	}
#endif
}

void LightingApp::loadShaders()
{
	try {
#if USE_RESOURCES
		mShader = gl::GlslProg::create( loadResource( RES_VERT_LIGHTING ), loadResource( RES_FRAG_LIGHTING ) );
		mShaderDepth = gl::GlslProg::create( loadResource( RES_VERT_DEPTH ), loadResource( RES_FRAG_DEPTH ) );
		mShaderPostProcessing = gl::GlslProg::create( loadResource( RES_VERT_POSTPROCESSING ), loadResource( RES_FRAG_POSTPROCESSING ) );
#else
		mShader = gl::GlslProg::create( loadAsset( "shaders/lighting.vert" ), loadAsset( "shaders/lighting.frag" ) );
		mShaderDepth = gl::GlslProg::create( loadAsset( "shaders/depth.vert" ), loadAsset( "shaders/depth.frag" ) );
		mShaderPostProcessing = gl::GlslProg::create( loadAsset( "shaders/postprocessing.vert" ), loadAsset( "shaders/postprocessing.frag" ) );
#endif
	}
	catch( const std::exception& e ) {
		console() << e.what() << std::endl;
		quit();
	}

	if( !mShaderDepth )
		mShaderDepth = gl::getStockShader( gl::ShaderDef() );

	if( !mShader )
		mShader = gl::getStockShader( gl::ShaderDef() );

	if( mBatch )
		mBatch->setGlslProg( mShader );
}

void LightingApp::loadMesh( DataSourceRef source )
{
	try {
		TriMesh mesh = TriMesh( TriMesh::Format().normals() );
		mesh.read( source );
		mBatch = gl::Batch::create( mesh, mShader );

		mBatchScale = 4.267f / mesh.calcBoundingBox().getSize().y;
	}
	catch( const std::exception& e ) {
		console() << e.what() << std::endl;
	}
}

void LightingApp::showTitle( const std::string& title )
{
	hideTitle();

	mTitleNext = title;

	timeline().add( [&]() { mTitleText = mTitleNext; }, timeline().getCurrentTime() + 0.5f );
	timeline().appendTo( &mTitleAlpha, 1.0f, 1.0f ).delay( 1.0f );
	//timeline().appendTo( &mTitleAlpha, 0.0f, 1.0f ).delay( 30.0f );

	mShowTitle = false;
}

void LightingApp::hideTitle()
{
	if( mTitleNext.empty() || mTitleAlpha() == 0.0f )
		return;

	mTitleNext = std::string();

	timeline().apply( &mTitleAlpha, 0.0f, 0.5f );
}

void LightingApp::drawTitle()
{
	const int w = getWindowWidth();
	const int h = getWindowHeight();

	vec2 textSize = mTitle->measureString( mTitleText );

	gl::enableAlphaBlending( true );
	gl::color( mTitleAlpha, mTitleAlpha, mTitleAlpha, 0.0f );
	mTitle->drawString( mTitleText, vec2( ( w - textSize.x ) / 2, h - textSize.y - 50 ), gl::TextureFont::DrawOptions() );
	gl::disableAlphaBlending();
}

CINDER_APP_NATIVE( LightingApp, RendererGl )
