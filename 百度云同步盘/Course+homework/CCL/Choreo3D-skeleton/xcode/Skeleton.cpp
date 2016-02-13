//  Created by Stream Gao on 8/30/15.

#include "Skeleton.h"

Skeleton::Skeleton(){}

Skeleton::Skeleton(const std::vector<glm::vec3>& jointPos)
{
    boneJoints = jointPos;
    
    head = boneJoints[0];
    r_shoulder = boneJoints[6];
    r_elbow = boneJoints[8];
    r_hand = boneJoints[9];
    l_shoulder = boneJoints[13];
    l_elbow = boneJoints[16];
    l_hand = boneJoints[17];
    spine_base = boneJoints[27];
    spine_top = boneJoints[21];
    r_hip = boneJoints[28];
    r_knee = boneJoints[34];
    r_foot = boneJoints[35];
    l_hip = boneJoints[29];
    l_knee = boneJoints[43];
    l_foot = boneJoints[44];
    
    physicsJoints.push_back(Joint(boneJoints[0],0, -1,1,-1,-1,"head"));
    physicsJoints.push_back(Joint(boneJoints[21],10, 0,2,5,6,"spine_top"));
    physicsJoints.push_back(Joint(l_shoulder,21,1,3,-1,-1,"l_shoulder"));
    physicsJoints.push_back(Joint(l_elbow,32,2,4,-1,-1,"l_elbow"));
    physicsJoints.push_back(Joint(l_hand,43,3,-1,-1,-1,"l_hand"));
    physicsJoints.push_back(Joint(boneJoints[27],20,1,6,-1,9,"spine_base"));
    physicsJoints.push_back(Joint(boneJoints[29],31,5,7,-1,-1,"l_hip"));
    physicsJoints.push_back(Joint(boneJoints[43],42,6,8,-1,-1,"l_knee"));
    physicsJoints.push_back(Joint(boneJoints[44],53,7,-1,-1,-1,"l_foot"));
    physicsJoints.push_back(Joint(r_hip,-31,5,-1,-1,10,"r_hip"));
    physicsJoints.push_back(Joint(r_knee,-42,9,-1,-1,11,"r_knee"));
    physicsJoints.push_back(Joint(r_foot,-53,10,-1,-1,-1,"r_foot"));
    physicsJoints.push_back(Joint(r_shoulder,-21,1,-1,-1,13,"r_shoulder"));
    physicsJoints.push_back(Joint(r_elbow,-32,12,-1,-1,14,"r_elbow"));
    physicsJoints.push_back(Joint(r_hand,-43,13,-1,-1,-1,"r_hand"));
    
    
    physicsBones.push_back(Bone(&physicsJoints[0], &physicsJoints[1], distance(physicsJoints[0].location, physicsJoints[1].location)));
    physicsBones.push_back(Bone(&physicsJoints[1], &physicsJoints[2], distance(physicsJoints[1].location, physicsJoints[2].location)));
    physicsBones.push_back(Bone(&physicsJoints[2], &physicsJoints[3], distance(physicsJoints[2].location, physicsJoints[3].location)));
    physicsBones.push_back(Bone(&physicsJoints[3], &physicsJoints[4], distance(physicsJoints[3].location, physicsJoints[4].location)));
    physicsBones.push_back(Bone(&physicsJoints[1], &physicsJoints[5], distance(physicsJoints[1].location, physicsJoints[5].location)));
    
    
    physicsBones.push_back(Bone(&physicsJoints[5], &physicsJoints[6], distance(physicsJoints[5].location, physicsJoints[6].location)));
    physicsBones.push_back(Bone(&physicsJoints[6], &physicsJoints[7], distance(physicsJoints[6].location, physicsJoints[7].location)));
    physicsBones.push_back(Bone(&physicsJoints[7], &physicsJoints[8], distance(physicsJoints[7].location, physicsJoints[8].location)));
    
    physicsBones.push_back(Bone(&physicsJoints[5], &physicsJoints[9], distance(physicsJoints[5].location, physicsJoints[9].location)));
    physicsBones.push_back(Bone(&physicsJoints[9], &physicsJoints[10], distance(physicsJoints[9].location, physicsJoints[10].location)));
    physicsBones.push_back(Bone(&physicsJoints[10], &physicsJoints[11], distance(physicsJoints[10].location, physicsJoints[11].location)));
    physicsBones.push_back(Bone(&physicsJoints[1], &physicsJoints[12], distance(physicsJoints[1].location, physicsJoints[12].location)));
    physicsBones.push_back(Bone(&physicsJoints[12], &physicsJoints[13], distance(physicsJoints[12].location, physicsJoints[13].location)));
    physicsBones.push_back(Bone(&physicsJoints[13], &physicsJoints[14], distance(physicsJoints[13].location, physicsJoints[14].location)));
    
    physicsBones.push_back(Bone(&physicsJoints[2], &physicsJoints[12], distance(physicsJoints[2].location, physicsJoints[12].location)));
    physicsBones.push_back(Bone(&physicsJoints[6], &physicsJoints[9], distance(physicsJoints[6].location, physicsJoints[9].location)));
    
    
}

void Skeleton::render(bool originOn)
{
    for (auto &j : physicsJoints){
        j.display();
    }
    
    for (int i=0;i<physicsBones.size()-2;i++){
        physicsBones.at(i).display();
    }
    
    if (originOn) {
        ci::gl::color(0.5,0.5,1.);
        ci::gl::lineWidth(5);
        
        //TORSO
        ci::gl::drawLine(head, spine_top);
        ci::gl::drawLine(r_shoulder, l_shoulder);
        ci::gl::drawLine(spine_top, spine_base);
        ci::gl::drawLine(spine_base, r_hip);
        ci::gl::drawLine(spine_base, l_hip);
        
        //RIGHT ARM
        ci::gl::drawLine(r_shoulder, r_elbow);
        ci::gl::drawLine(r_elbow, r_hand);
        
        //LEFT ARM
        ci::gl::drawLine(l_shoulder, l_elbow);
        ci::gl::drawLine(l_elbow, l_hand);
        
        //RIGHT LEG
        ci::gl::drawLine(r_hip, r_knee);
        ci::gl::drawLine(r_knee, r_foot);
        
        //LEFT LEG
        ci::gl::drawLine(l_hip, l_knee);
        ci::gl::drawLine(l_knee, l_foot);
        
        
    }
}



void Skeleton::update(const std::vector<glm::vec3>& pos){
    
    boneJoints = pos;
    
    head = boneJoints[0];
    r_shoulder = boneJoints[6];
    r_elbow = boneJoints[8];
    r_hand = boneJoints[9];
    l_shoulder = boneJoints[13];
    l_elbow = boneJoints[16];
    l_hand = boneJoints[17];
    spine_top = boneJoints[21];
    spine_base = boneJoints[27];
    r_hip = boneJoints[28];
    r_knee = boneJoints[34];
    r_foot = boneJoints[35];
    l_hip = boneJoints[29];
    l_knee = boneJoints[43];
    l_foot = boneJoints[44];
    
    physicsJoints[0].update1( head );
    physicsJoints[1].update1( spine_top);
    physicsJoints[2].update1( l_shoulder );
    physicsJoints[3].update1( l_elbow );
    physicsJoints[4].update1( l_hand );
    physicsJoints[5].update1( spine_base);
    physicsJoints[6].update1( l_hip );
    physicsJoints[7].update1( l_knee );
    physicsJoints[8].update1( l_foot );
    physicsJoints[9].update1( r_hip );
    physicsJoints[10].update1( r_knee );
    physicsJoints[11].update1( r_foot );
    physicsJoints[12].update1( r_shoulder );
    physicsJoints[13].update1( r_elbow );
    physicsJoints[14].update1( r_hand );
    
    for (auto &b : physicsBones){
        b.update();
    }
}


void Skeleton::pushone(glm::vec3 f){
    physicsJoints[5].applyForce( f );
}


void Skeleton::push(glm::vec3 f, int k){
    
    glm::vec3 newf= glm::vec3(1000, -200, 1000);
    
    switch (k) {
        case 0: //fall
            for (int i=0;i<physicsJoints.size();i++){
                newf =f;
                newf*= 50/(std::abs(physicsJoints.at(i).degree)+100);
                //physicsJoints.at(i).applyForce(newf);
                physicsJoints.at(i).applyForce( glm::vec3(0,-1980,0) );
            }
            break;
        case 32: //fall to the ground    circling	motion   space
            for (int i=0;i<physicsJoints.size(); i++) {
                physicsJoints.at(i).applyForce(f);
                physicsJoints.at(i).applyForce( glm::vec3(0,-1980,0) );
            }
        case 49:  //a different directions
            physicsJoints.at(1).applyForce(f);
            f*=-1;
            //physicsJoints.at(2).applyForce(f);
            break;
        case 50: //b
            for (auto &j: physicsJoints) {
                j.applyForce( glm::vec3(1000, -100, 1000) );
            }
            break;
        default:
            if(k<physicsJoints.size())   physicsJoints[k].applyForce(f);
            break;
    }
}





void Skeleton::drawRibbon()
{
}

