//  Marker.h
//  CinderProject
//
//  Created by StreamGao on 8/29/15.


#pragma once

class Joint {
public:
    std::string name;
    //Joint *lchild,*mchild,*rchild, *parent;
    int lchild, mchild, rchild, parent;
    int degree;
    float m = 8;
    float damp = 0.5;
    bool drag = false;
    
    ci::vec3 location;
    ci::vec3 velocity;
    ci::vec3 acceleration;
    ci::vec3 dragOffset;
    
    Joint();
    Joint(ci::vec3 l);
    Joint(ci::vec3 loc, int d, int p, int l, int m, int r, std::string n);
    
    void display();
    void update();
    void applyForce(ci::vec3 f);
    
    void click(ci::vec3 po);
    void dragged(ci::vec3 po);
    
    //    void push(ci::vec3 f, Node *n);
    //    void connect(Node *par);
};
