#include "Body.h"


class Body{
public:
    Body();
    Body(Node *root);
    Node *n;
    Node root;
    
    void push( ci::vec3 f, Node *n);
    
};



Body::Body( Node *root){   //top spine
// first root
// second top head
    std::string s;
    //read marker name
    //if not null
    //location, 
    root = new Node();
    if(s!="L_Pinky"){
    
    }
}





/*
 typedef struct BiNode{
 char data;
 vec2 location;
 struct BiNode *lchild,*rchild, *parent;
 
 }BiTNode,*BiTree;
 
 void CreateBiTree(BiTree &T){
 char ch;
 cin>>ch;
 if (ch=='#')
 T=NULL;
 else
 {
 T=new BiNode;
 T->data=ch;
 CreateBiTree(T->lchild);
 CreateBiTree(T->rchild);
 }
 }
 
 void PreOrderTraverse(BiTree T){
 if(T){
 cout<<T->data;
 PreOrderTraverse(T->lchild);
 PreOrderTraverse(T->rchild);
 }
 }
 */