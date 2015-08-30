#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"
#include "Bone.h"

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "skeletonParse.h"


using namespace ci;
using namespace ci::app;
using namespace std;
using namespace boost;


class CinderProjectApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void mouseUp(MouseEvent event) override;
    void mouseDrag(MouseEvent event) override;
    
	void update() override;
	void draw() override;
    void loadData();
    
    std::array<Joint, 16> joints;
    std::array<Bone, 15> bones;
    ci::vec3 mousepo;
};

void CinderProjectApp::setup()
{
    loadData();
}

void CinderProjectApp::mouseDown( MouseEvent event )
{
    mousepo = vec3( event.getPos().x, event.getPos().y, 0);
    for(Joint &j: joints){
        j.click( vec3(event.getPos().x, event.getPos().y, 0 ) );
    }
}


void CinderProjectApp::mouseUp( MouseEvent event )
{
    for(Joint &j: joints){
        j.drag=false;
    }
}

void CinderProjectApp::mouseDrag(MouseEvent event){
    mousepo = vec3( event.getPos().x, event.getPos().y, 0);
}



void CinderProjectApp::update()
{
}

void CinderProjectApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::setMatricesWindowPersp(getWindowSize());
    gl::enableDepthRead();
    
    for (Bone &b : bones) {
        b.update();
        b.display();    }
    for (Joint &j : joints) {
        j.update();
        j.display();
        j.dragged( mousepo );
    }
}


void CinderProjectApp::loadData(){
    
    FILE* f;
    const char* gFile = "skeleton1.csv";
    char mystring[350];
    string mstring;
    
    string pathToFile = getAssetPath(fs::path( gFile )).string();
    if(fs::exists(pathToFile))
    {
        f = fopen(pathToFile.c_str(), "r");
        fgets (mystring,350,f);
        fclose (f);
        
        std::vector< std::string > lines = split(mystring,'\r',false);
    
        for (int i=0; i<lines.size(); i++){
            
                std::vector< std::string > tjoint = split(lines.at(i),',',false);
                
                int p= getParent(tjoint);
                int de = getDegree(tjoint);
                
                joints[i]= Joint(vec3(getWindowWidth()/2+40*(de%10), 5*abs(de)+50, 0), de,p, getLchild(tjoint), getMchild(tjoint), getRchild(tjoint), tjoint.at(5));
                
                
                if(p!=-1){
                    float length = distance( joints[i].location, joints[p].location);
                    bones[i-1] = Bone( &joints[p], &joints[i], length);
                  
                }
            }
        
    }else{
        console() << "File aint there buddy!" << std::endl;
    }

    
}

CINDER_APP_NATIVE( CinderProjectApp, RendererGl )
