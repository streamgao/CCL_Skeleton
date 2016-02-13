//  Created by Stream Gao on 8/30/15.

#include "Joint.h"

class Bone{
public:
    float len;
    float k=7;
    Joint *a,*b;
    
    Bone();
    Bone(Joint *_a, Joint *_b, float l);
    
    void display();
    void update();
    void lengthconstrain();
    
};



