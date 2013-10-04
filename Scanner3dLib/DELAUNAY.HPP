#include <math.h>
#ifndef TRIANGULATION
#define TRIANGULATION

class dEdge;
class Triangle;

class Node{
public:
     float   x,y,z;       // coordinate X,Y
     dEdge * anEdge;     // an edge which start from this node
	 void *pnt;// pointer to a point
     char  type;      // 1: inner nodes, 2: on convex hull, 3: ->->->
	 Node(){}
      Node(float x, float y,float z) {this->x=x; this->y=y;this->z=z;anEdge=0;pnt=0;}
      dEdge * GetEdge() { return anEdge;}
      double Distance(double px, double py){ 
		  double dx=px-x;
         double dy=py-y;
         //return (sqrt(dx*dx+dy*dy));
		 return ((dx*dx+dy*dy));
       }
};

class dEdge{
public:
     Node *        p1,*p2;         // start and end point of the edge
     dEdge *         invE;     // inverse edge (p2->p1)
     dEdge *         nextE;    // next edge in the triangle in countclickwise
     dEdge *         nextH;    // convex hull link
     Triangle     *inT;      // triangle containing this edge
     double a,b,c;               // line equation parameters-> aX+bY+c=0

     dEdge (Node *p1, Node *p2) { 
		 Update(p1,p2); 
		invE=0;
		nextE=0;
		nextH=0;
		inT = 0;
	 }

    inline void Update(Node *p1, Node *p2) { this->p1=p1; this->p2=p2; Setabc(); AsIndex(); }
    inline Triangle *Tri() { return inT;}
    inline void setNextE(dEdge * e) {nextE=e;}
    inline void setNextH(dEdge * e) {nextH=e;}
    inline void setTri(Triangle *t) {inT=t;}
    inline void setInvE(dEdge * e) {invE=e;}
    inline dEdge * MakeSymm() { dEdge * e=new dEdge(p2,p1); LinkSymm(e); return e; }
    inline void LinkSymm(dEdge * e) { this->invE=e; if(e!=0) e->invE=this; }
     inline int onSide(Node *nd)
       { double s=a*nd->x+b*nd->y+c;
         if(s>0.0) return 1;
         if(s<0.0) return -1;
         return 0;
       }
    inline void Setabc()   // set parameters of a,b,c
    {   a=p2->y-p1->y;
		b=p1->x-p2->x; 
		c=p2->x*p1->y-p1->x*p2->y; 
	}

    inline void AsIndex()  { p1->anEdge=this;}
    inline dEdge * MostLeft()
       { dEdge * ee,*e=this;
         while((ee=e->nextE->nextE->invE)!=0 && ee!=this) e=ee;
         return e->nextE->nextE;
       }
    inline dEdge * MostRight()
       { dEdge * ee,*e=this;
         while(e->invE!=0 && (ee=e->invE->nextE)!=this) e=ee;
         return e;
       }
};

class Triangle{
public:
     dEdge *anEdge;      // an edge of this triangle
     double  c_cx;        // center of circle: X
     double  c_cy;        // center of circle: Y
     double  c_r;         // radius of circle
       Triangle(List *edges, dEdge * e1, dEdge * e2, dEdge * e3){
         Update(e1,e2,e3);
         edges->Add(e1);
         edges->Add(e2);
         edges->Add(e3);
       }

       Triangle(dEdge * e1, dEdge * e2, dEdge * e3) {
		   Update(e1,e2,e3);
	   }
      void Update(dEdge * e1, dEdge * e2, dEdge * e3)
       {
         anEdge=e1;
         e1->setNextE(e2);
         e2->setNextE(e3);
         e3->setNextE(e1);
         e1->setTri(this);
         e2->setTri(this);
         e3->setTri(this);
         FindCircle();
       }

      dEdge * GetEdge() { return anEdge;}
     bool InCircle(Node *nd) { return nd->Distance(c_cx,c_cy)<c_r; }
     void RemoveEdges(List *edges)
       {
         edges->Remove(anEdge);
         edges->Remove(anEdge->nextE);
         edges->Remove(anEdge->nextE->nextE);
       }
     void FindCircle()
       {
         double x1=(double) anEdge->p1->x;
         double y1=(double) anEdge->p1->y;
         double x2=(double) anEdge->p2->x;
         double y2=(double) anEdge->p2->y;
         double x3=(double) anEdge->nextE->p2->x;
         double y3=(double) anEdge->nextE->p2->y;
         double a=(y2-y3)*(x2-x1)-(y2-y1)*(x2-x3);
         double a1=(x1+x2)*(x2-x1)+(y2-y1)*(y1+y2);
         double a2=(x2+x3)*(x2-x3)+(y2-y3)*(y2+y3);
         c_cx=(a1*(y2-y3)-a2*(y2-y1))/a/2;
         c_cy=(a2*(x2-x1)-a1*(x2-x3))/a/2;
         c_r=anEdge->p1->Distance(c_cx,c_cy);
       }
};

class DelaunayT{
public:
   List *nodes;        // nodes set
   List *edges;        // edges set
   List *tris;         // triangles set
   dEdge *   hullStart;    // entring edge of convex hull
   dEdge *   actE;

    DelaunayT(int size){
       tris=new List;//(size);
       nodes=new List;//(3*size);
       edges=new List;//(3*size);
     }

    void Clear()
     {
       nodes->Destroy();//>removeAllElements();
       edges->Destroy();//removeAllElements();
       tris->Destroy();//removeAllElements();
     }

    void Insert(float px, float py, float pz)
      {
        int eid;
        Node *nd=new Node(px,py,pz);
        nodes->Add(nd);
        if(nodes->Count()<3) return;
        if(nodes->Count()==3)    // create the first triangle
          {
            Node *p1=(Node *)nodes->GetItem(0);
            Node *p2=(Node *)nodes->GetItem(1);
            Node *p3=(Node *)nodes->GetItem(2);
            dEdge * e1=new dEdge(p1,p2);
            if(e1->onSide(p3)==0) { nodes->Remove(nd); return; }
            if(e1->onSide(p3)==-1)  // right side
              {
                p1=(Node *)nodes->GetItem(1);
                p2=(Node *)nodes->GetItem(0);
                e1->Update(p1,p2);
              }
            dEdge * e2=new dEdge(p2,p3);
            dEdge * e3=new dEdge(p3,p1);
            e1->setNextH(e2);
            e2->setNextH(e3);
            e3->setNextH(e1);
            hullStart=e1;
            tris->Add(new Triangle(edges,e1,e2,e3));
            return;
          }
        actE=(dEdge *)edges->GetItem(0);
        if(actE->onSide(nd)==-1)
          { if(actE->invE==0) eid=-1;
            else eid=SearchEdge(actE->invE,nd);
          }
        else eid=SearchEdge(actE,nd);
        if(eid==0) { nodes->Remove(nd); return; }
        if(eid>0) ExpandTri(actE,nd,eid);   // nd is inside or on a triangle
        else ExpandHull(nd);                // nd is outside convex hull
      }
/*
    public void Delete(int px, int py)

      {

        if(nodes->size()<=3) return;   // not allow deletion for only 1 triangle

        Node *nd=Nearest((double)px,(double)py);

        if(nd==0) return;          // not found

        nodes->Remove(nd);

        dEdge * e,ee,start;

        start=e=nd->GetEdge()->MostRight();

        int nodetype=0;

        int idegree=-1;

        dEdge * index[]=new dEdge[100];

        while(nodetype==0)

         {

           edges->Remove(ee=e->nextE);

           index[++idegree]=ee;

           ee->AsIndex();

           tris->Remove(e->Tri());   // delete triangles involved

           edges->Remove(e);

           edges->Remove(ee->nextE);

           e=ee->nextE->invE;            // next left edge

           if(e==0) nodetype=2;         // nd on convex hull

           if(e==start) nodetype=1;        // inner node

         }

        // generate new triangles and add to triangulation

        int cur_i=0,cur_n=0;

        int last_n=idegree;

        dEdge * e1=0,e2=0,e3;

        while(last_n>=1)

          {

             e1=index[cur_i];

             e2=index[cur_i+1];

             if(last_n==2 && nodetype==1)

               {

                 tris->Add(new Triangle(edges,e1,e2,index[2]));

                 SwapTest(e1);

                 SwapTest(e2);

                 SwapTest(index[2]);

                 break;

               }

             if(last_n==1 && nodetype==1)

               {

                 index[0]->invE->LinkSymm(index[1]->invE);

                 index[0]->invE->AsIndex();

                 index[1]->invE->AsIndex();

                 SwapTest(index[0]->invE);

                 break;

               }

             if(e1->onSide(e2->p2)==1)  // left side

               {

                 e3=new dEdge(e2->p2,e1->p1);

                 cur_i+=2;

                 index[cur_n++]=e3->MakeSymm();

                 tris->Add(new Triangle(edges,e1,e2,e3));

                 SwapTest(e1);

                 SwapTest(e2);

               }

             else index[cur_n++]=index[cur_i++];

             if(cur_i==last_n) index[cur_n++]=index[cur_i++];

             if(cur_i==last_n+1)

               {

                 if(last_n==cur_n-1) break;

                 last_n=cur_n-1;

                 cur_i=cur_n=0;

               }

          }

        if(nodetype==2)   // reconstruct the convex hull

         {

           index[last_n]->invE->MostLeft()->setNextH(hullStart=index[last_n]->invE);

           for(int i=last_n;i>0;i--)

             { index[i]->invE->setNextH(index[i-1]->invE);

               index[i]->invE->setInvE(0);

             }

           index[0]->invE->setNextH(start->nextH);

           index[0]->invE->setInvE(0);

         }

     }
*/
void ExpandTri(dEdge * e, Node *nd, int type)
     {
      dEdge * e1=e;
       dEdge * e2=e1->nextE;
       dEdge * e3=e2->nextE;
       Node *p1=e1->p1;
       Node *p2=e2->p1;
       Node *p3=e3->p1;
       if(type==2)    // nd is inside of the triangle
         {
          dEdge * e10=new dEdge(p1,nd);
          dEdge * e20=new dEdge(p2,nd);
          dEdge * e30=new dEdge(p3,nd);
          e->Tri()->RemoveEdges(edges);
          tris->Remove(e->Tri());     // remove old triangle
          tris->Add(new Triangle(edges,e1,e20,e10->MakeSymm()));
          tris->Add(new Triangle(edges,e2,e30,e20->MakeSymm()));
          tris->Add(new Triangle(edges,e3,e10,e30->MakeSymm()));
          SwapTest(e1);   // swap test for the three new triangles
          SwapTest(e2);
          SwapTest(e3);
         }
       else           // nd is on the edge e
         {
          dEdge * e4=e1->invE;
          if(e4==0 || e4->Tri()==0)           // one triangle involved
            {
              dEdge * e30=new dEdge(p3,nd);
              dEdge * e02=new dEdge(nd,p2);
              dEdge * e10=new dEdge(p1,nd);
              dEdge * e03=e30->MakeSymm();
              e10->AsIndex();
              e1->MostLeft()->setNextH(e10);
              e10->setNextH(e02);
              e02->setNextH(e1->nextH);
              hullStart=e02;
              tris->Remove(e1->Tri());                   // remove oldtriangle               // add two new triangles
              edges->Remove(e1);
              edges->Add(e10);
              edges->Add(e02);
              edges->Add(e30);
              edges->Add(e03);
              tris->Add(new Triangle(e2,e30,e02));
              tris->Add(new Triangle(e3,e10,e03));
              SwapTest(e2);   // swap test for the two new triangles
              SwapTest(e3);
              SwapTest(e30);
            }
          else         // two triangle involved
            {
              dEdge * e5=e4->nextE;
              dEdge * e6=e5->nextE;
              Node *p4=e6->p1;
              dEdge * e10=new dEdge(p1,nd);
              dEdge * e20=new dEdge(p2,nd);
              dEdge * e30=new dEdge(p3,nd);
              dEdge * e40=new dEdge(p4,nd);
              tris->Remove(e->Tri());                   // remove oldtriangle
              e->Tri()->RemoveEdges(edges);
              tris->Remove(e4->Tri());               // remove old triangle
              e4->Tri()->RemoveEdges(edges);
              e5->AsIndex();   // because e, e4 removed, reset edge index of node p1 and p2
              e2->AsIndex();
              tris->Add(new Triangle(edges,e2,e30,e20->MakeSymm()));
              tris->Add(new Triangle(edges,e3,e10,e30->MakeSymm()));
              tris->Add(new Triangle(edges,e5,e40,e10->MakeSymm()));
              tris->Add(new Triangle(edges,e6,e20,e40->MakeSymm()));
              SwapTest(e2);   // swap test for the three new triangles
              SwapTest(e3);
              SwapTest(e5);
              SwapTest(e6);
              SwapTest(e10);
              SwapTest(e20);
              SwapTest(e30);
              SwapTest(e40);
            }
         }
     }
   void ExpandHull(Node *nd)
     {
        dEdge * e1,*e2,*e3=0,*enext;
        dEdge * e=hullStart;
        dEdge * comedge=0,*lastbe=0;
        while(true)
          {
            enext=e->nextH;
            if(e->onSide(nd)==-1)   // right side
              {
                if(lastbe!=0)
                  {
                     e1=e->MakeSymm();
                     e2=new dEdge(e->p1,nd);
                     e3=new dEdge(nd,e->p2);
                     if(comedge==0)
                       {
                         hullStart=lastbe;
                         lastbe->setNextH(e2);
                         lastbe=e2;
                       }
                     else comedge->LinkSymm(e2);
                     comedge=e3;
                     tris->Add(new Triangle(edges,e1,e2,e3));
                     SwapTest(e);
                  }
              }
            else
              {
                if(comedge!=0) break;
                lastbe=e;
              }
            e=enext;
          }
        lastbe->setNextH(e3);
        e3->setNextH(e);
     }

   int SearchEdge(dEdge * e, Node *nd)
     {
      int f2,f3;
      dEdge * e0=0;
      if((f2=e->nextE->onSide(nd))==-1)
        { if(e->nextE->invE!=0) return SearchEdge(e->nextE->invE,nd);
          else { actE=e; return -1;}
        }
      if(f2==0) e0=e->nextE;
      dEdge * ee=e->nextE;
      if((f3=ee->nextE->onSide(nd))==-1)
        { if(ee->nextE->invE!=0) return SearchEdge(ee->nextE->invE,nd);
          else { actE=ee->nextE; return -1;}
        }
      if(f3==0) e0=ee->nextE;
      if(e->onSide(nd)==0) e0=e;
      if(e0!=0)
        {
          actE=e0;
          if(e0->nextE->onSide(nd)==0) {actE=e0->nextE; return 0;}
          if(e0->nextE->nextE->onSide(nd)==0) return 0;
          return 1;
        }
      actE=ee;
      return 2;
     }

   void SwapTest(dEdge * e11)
     {
       dEdge * e21=e11->invE;
       if(e21==0 || e21->Tri()==0) return;
       dEdge * e12=e11->nextE;
       dEdge * e13=e12->nextE;
       dEdge * e22=e21->nextE;
       dEdge * e23=e22->nextE;
       if(e11->Tri()->InCircle(e22->p2) || e21->Tri()->InCircle(e12->p2))
         {
           e11->Update(e22->p2,e12->p2);
           e21->Update(e12->p2,e22->p2);
           e11->LinkSymm(e21);
           e13->Tri()->Update(e13,e22,e11);
           e23->Tri()->Update(e23,e12,e21);
           e12->AsIndex();
           e22->AsIndex();
           SwapTest(e12);
           SwapTest(e22);
           SwapTest(e13);
           SwapTest(e23);
         }
     }

   Node *Nearest(double x, double y){
       // locate a node nearest to (px,py)
       double dismin=0.0,s;
       Node *nd=0;
	   ListItem *li;
	   li  = nodes->list;
       for(int i=0;i<nodes->Count();i++){
			s = ((Node *)(li->data))->Distance (x,y);
           //s=(nodes->GetItem(i))->Distance(x,y);
           if(s<dismin||nd==0) { 
			   dismin=s;
			//	   nd=(Node *)nodes->GetItem(i);
			   nd = (Node *)(li->data);
		   }
		   li = li->next; 
         }
       return nd;
     }
};

#endif