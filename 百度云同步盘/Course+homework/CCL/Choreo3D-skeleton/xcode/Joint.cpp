//  Created by Stream Gao on 8/30/15.
//
//
#include "Joint.h"

using namespace ci;

Joint::Joint(){
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
}


Joint::Joint(ci::vec3 loc, int d, int p, int l, int m, int r, std::string n){
    location = loc;
    degree =d; //wont need this
    parent = p;
    lchild=l;
    mchild=m;
    rchild=r;
    name=n;
    
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
}

Joint::Joint(glm::vec3 loc, int p, int l, int m, int r, std::string n){
    
    location = loc;
    parent = p;
    lchild=l;
    mchild=m;
    rchild=r;
    name=n;
    
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
}


Joint::Joint(glm::vec3 loc){
    
    location = loc;
    
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
    
    isDataDriven = true;
}

Joint::Joint(glm::vec3 loc, std::string n){
    name =n;
    location = loc;
    
    velocity = vec3(0.0,0.0,0.0);
    acceleration = vec3(0.0,0.0,0.0);
    offsetLoc = vec3(0.0,0.0,0.0);
    
    isDataDriven = true;
}


void Joint::display(){
    gl::color(0.68, 0.68, 0.68);
    gl::drawColorCube(location, vec3(10*m,10*m,10*m));
}

void Joint::update(){
    velocity+=acceleration;
    velocity*=damp;
    if((location.y+offsetLoc.y)>0){
        //if( distance(offsetLoc, velocity) < distance(offsetLoc,vec3(0,0,0)) )     //not bounce back
        offsetLoc+=velocity;
        location+=offsetLoc;
    }else
        location.y=0;
    
    acceleration*=0;
    
}


void Joint::update1( glm::vec3 newloc ){
    if( distance(location, newloc)<1000 && distance(location, newloc)>-1000)
        location = newloc;
    
    velocity+=acceleration;
    velocity*=damp;
    
    if( (location.y+offsetLoc.y)> -10 ){
        //if( distance(offsetLoc, velocity) < distance(offsetLoc,vec3(0,0,0)) )     //not bounce back
        offsetLoc+=velocity;
        location+=offsetLoc;
    }else{
        location.y=0;
        offsetLoc =  glm::vec3(0,0,0);
    }
    
    acceleration*=0;
}


void Joint::applyForce(glm::vec3 f){
    vec3 force =f;
    force/=m;
    acceleration+=force;
    
}






