#include "Joint.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Joint::Joint(){
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    dragOffset = vec3(0.0,0.0,0.0);
}

Joint::Joint(ci::vec3 loc, int d, int p, int l, int m, int r, std::string n){
    location = loc;
    degree =d;
    parent = p;
    lchild=l;
    mchild=m;
    rchild=r;
    name=n;
    
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    dragOffset = vec3(0.0,0.0,0.0);
}

void Joint::display(){
    gl::color(0.68, 0.68, 0.68);
    gl::drawColorCube(location, vec3(m,m,m));
    
}

void Joint::update(){
    velocity+=(acceleration);
    velocity*=damp;
    location+=velocity;
    acceleration*=0;
}

void Joint::applyForce(ci::vec3 f){
    vec3 force =f;
    force/=m;
    acceleration+=force;
}


void Joint::click(ci::vec3 po){
    
    float d = distance(po, location);
    if (d < m) {   //if less than the radius
        drag = true;
        cinder::app::console()<<"set drag true"<<drag<<std::endl;
        dragOffset.x = location.x-po.x;
        dragOffset.y = location.y-po.y;
        dragOffset.z = location.z-po.z;
    }
}


void Joint::dragged(ci::vec3 po){
    if(drag){
        location = po+ dragOffset;
    }
}









