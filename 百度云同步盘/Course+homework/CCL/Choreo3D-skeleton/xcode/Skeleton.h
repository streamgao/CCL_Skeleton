//  Created by Stream Gao on 8/30/15.
//

#pragma once

#include <stdio.h>
#include "Joint.h"
#include "Bone.h"

class Skeleton{
public:
    Skeleton();
    Skeleton(const std::vector<glm::vec3>& jointPos);
    
    std::vector<glm::vec3>boneJoints;
    
    std::vector<Joint> physicsJoints;
    std::vector<Bone> physicsBones;
    
    glm::vec3 head, r_shoulder, r_elbow, r_hand, l_shoulder, l_elbow, l_hand, spine_base, spine_top, r_hip, r_knee, r_foot, l_hip, l_knee, l_foot;
    
    void render(bool originOn);
    void update(const std::vector<glm::vec3>& positions);
    
    ci::gl::VboRef              mSkeletonVbo;
    ci::gl::VboMeshRef          mSkeletonMesh;
    
    void pushone( glm::vec3 f);
    void push( glm::vec3 f, int i);
    void drawRibbon();
};


