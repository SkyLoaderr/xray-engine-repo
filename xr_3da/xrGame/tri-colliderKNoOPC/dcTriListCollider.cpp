// Do NOT build this file seperately. It is included in dTriList.cpp automatically.


#include "..\..\cl_intersect.h"


dcTriListCollider::dcTriListCollider(dxGeom* Geometry)
{

	this->Geometry = Geometry;

	GeomData = (dxTriList*)dGeomGetClassData(Geometry);

}



dcTriListCollider::~dcTriListCollider(){

	//

}





int dCollideBP (const dxGeom* o1, const dxGeom* o2, int flags, dContactGeom *contact, int skip);	// ODE internal function



#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))

struct Triangle {
	dReal* v0;
	dReal* v1;
	dReal* v2;
	dVector3 side0;
	dVector3 side1;
	dVector3 norm;
	dReal dist;
	dReal pos;
	dReal depth;
	bool compare (Triangle t1){
		
		if(t1.v0[0]!=v0[0] ||
		   t1.v0[1]!=v0[1] ||
		   t1.v0[2]!=v0[2] ||
		   t1.v1[0]!=v1[0] ||
		   t1.v1[1]!=v1[1] ||
		   t1.v1[2]!=v1[2] ||
		   t1.v2[0]!=v2[0] ||
		   t1.v2[1]!=v2[1] ||
		   t1.v2[2]!=v2[2]) return false;
		else return true;
		
	
	}
};
static dVector3 last_pos={dInfinity,0.f,0.f,0.f};
extern "C" int dSortTriCollide (
						const dxGeom *o1, const dxGeom *o2,
						int flags, dContactGeom *contact, int skip,
						CDB::RESULT*    R_begin,
						CDB::RESULT*    R_end ,
						CDB::TRI*       T_array
						)
{
int ret=0;
Triangle tri,neg_tri,b_neg_tri;
vector<Triangle> pos_tries;
vector<Triangle> neg_tries;
dReal pos_dist=dInfinity,neg_depth=dInfinity,b_neg_depth=dInfinity;
dReal max_proj=-dInfinity,proj;
const dReal* p=o1->pos;
dVector3 pos_vect={last_pos[0]-p[0],last_pos[1]-p[1],last_pos[2]-p[2]};
dxBox *box = (dxBox*) CLASSDATA(o1);
const dVector3 hside={box->side[0]/2.f,box->side[1]/2.f,box->side[2]/2.f,-1};
const dReal *R = o1->R;
for (CDB::RESULT* Res=R_begin; Res!=R_end; Res++)
 {
 CDB::TRI* T = T_array + Res->id;
 tri.v0=(dReal*)T->verts[0];
 tri.v1=(dReal*)T->verts[1];
 tri.v2=(dReal*)T->verts[2];
 tri.side0[0]=tri.v1[0]-tri.v0[0];
 tri.side0[1]=tri.v1[1]-tri.v0[1];
 tri.side0[2]=tri.v1[2]-tri.v0[2];
 tri.side1[0]=tri.v2[0]-tri.v1[0];
 tri.side1[1]=tri.v2[1]-tri.v1[1];
 tri.side1[2]=tri.v2[2]-tri.v1[2];
 dCROSS(tri.norm,=,tri.side0,tri.side1);
 dNormalize3(tri.norm);
 dReal sidePr=
	 dFabs(dDOT14(tri.norm,R+0)*hside[0])+
	 dFabs(dDOT14(tri.norm,R+1)*hside[1])+
	 dFabs(dDOT14(tri.norm,R+2)*hside[2]);

 tri.pos=dDOT(tri.v0,tri.norm);
 dReal ps=dDOT(p,tri.norm);
 tri.dist=dDOT(p,tri.norm)-tri.pos;
 tri.depth=sidePr-tri.dist;
 if(tri.dist<0.f){
	 if(TriContainPoint(tri.v0,tri.v1,tri.v2,
						tri.norm,tri.side0,
						tri.side1,p)

						){
			if(neg_depth>tri.depth){
									neg_depth=tri.depth;
									neg_tri=tri;
									}


						}
	 else{			if(b_neg_depth>tri.depth){
									b_neg_depth=tri.depth;
									b_neg_tri=tri;
									}
		}
		}
 else{
				  pos_tries.push_back(tri);
				  //if(tri.dist<pos_dist) pos_dist=tri.dist;
	}
 }
 
 vector<Triangle>::iterator i;
 //if( neg_dist==-dInfinity){


								  
 //}
 /*
 vector<Triangle>::iterator j;
if(max_proj==-dInfinity)
 for(i=neg_tries.begin();i!=neg_tries.end();i++){
	 bool include = true;
	 for(j=pos_tries.begin();j!=pos_tries.end();j++){
							//if(!(dDOT(i->norm,j->norm)>0.f))
								if((dDOT(i->norm,j->v0)-i->pos)<0.f||
								   (dDOT(i->norm,j->v1)-i->pos)<0.f||
								   (dDOT(i->norm,j->v2)-i->pos)<0.f
								   ){
									include=false;
									break;
								    }
	 };
		if(include)
			 			ret+=dSortedTriBox(i->side0,i->side1,i->norm,
								  i->v0,i->v1,i->v2,i->dist,
								  o1,o2,flags,
								  CONTACT(contact, ret * skip),
								  skip);
				
}
	
 */




				  
	 if(neg_depth<dInfinity&&ret==0){
		 	 bool include = true;

				for(i=pos_tries.begin();i!=pos_tries.end();i++){
					if(TriContainPoint(i->v0,i->v1,i->v2,
										i->norm,i->side0,
										i->side1,p))
								if((dDOT(neg_tri.norm,i->v0)-neg_tri.pos)<0.f||
								   (dDOT(neg_tri.norm,i->v1)-neg_tri.pos)<0.f||
								   (dDOT(neg_tri.norm,i->v2)-neg_tri.pos)<0.f
								   ){
									include=false;
									break;
								    }
				};
		if(include)		
				ret+=dSortedTriBox(neg_tri.side0,neg_tri.side1,neg_tri.norm,
								  neg_tri.v0,neg_tri.v1,neg_tri.v2,neg_tri.dist,
								  o1,o2,flags,
								  CONTACT(contact, ret * skip),
								  skip);	

		}

 //if(ret==0)
	 for(i=pos_tries.begin();i!=pos_tries.end();i++)
		 	 				ret+=dTriBox (i->v0,i->v1,i->v2,
								  o1,
								  o2,
								  3,
								  CONTACT(contact, ret * skip),   skip);


	 	 if(b_neg_depth<dInfinity&&ret==0){
		 	 bool include = true;
				for(i=pos_tries.begin();i!=pos_tries.end();i++){
								if((dDOT(b_neg_tri.norm,i->v0)-b_neg_tri.pos)<0.f||
								   (dDOT(b_neg_tri.norm,i->v1)-b_neg_tri.pos)<0.f||
								   (dDOT(b_neg_tri.norm,i->v2)-b_neg_tri.pos)<0.f
								   ){
									include=false;
									break;
								    }
				};
		if(include)		
				ret+=dSortedTriBox(b_neg_tri.side0,b_neg_tri.side1,b_neg_tri.norm,
								  b_neg_tri.v0,b_neg_tri.v1,b_neg_tri.v2,b_neg_tri.dist,
								  o1,o2,flags,
								  CONTACT(contact, ret * skip),
								  skip);	

		}

return ret;
}

extern "C" int dSortTriCollide1 (
						const dxGeom *o1, const dxGeom *o2,
						int flags, dContactGeom *contact, int skip,
						CDB::RESULT*    R_begin,
						CDB::RESULT*    R_end ,
						CDB::TRI*       T_array
						)
{
int ret=0;
Triangle tri,test_tri;
vector<Triangle> tries;
dReal test_dist=dInfinity;
const dReal* p=o1->pos;
for (CDB::RESULT* Res=R_begin; Res!=R_end; Res++)
 {
 CDB::TRI* T = T_array + Res->id;
// tri.v0=(dReal*)T->verts[0];
 //tri.v1=(dReal*)T->verts[1];
 //tri.v2=(dReal*)T->verts[2];
 tri.side0[0]=tri.v1[0]-tri.v0[0];
 tri.side0[1]=tri.v1[1]-tri.v0[1];
 tri.side0[2]=tri.v1[2]-tri.v0[2];
 tri.side1[0]=tri.v2[0]-tri.v1[0];
 tri.side1[1]=tri.v2[1]-tri.v1[1];
 tri.side1[2]=tri.v2[2]-tri.v1[2];
 dCROSS(tri.norm,=,tri.side0,tri.side1);
 dNormalize3(tri.norm);
 tri.dist=dDOT(p,tri.norm)-dDOT(tri.v0,tri.norm);
 if(dFabs(tri.dist)<test_dist){
								test_tri=tri;
								test_dist=dFabs(tri.dist);
								}
 tries.push_back(tri);
 }
 
 vector<Triangle>::iterator i;
	 for(i=tries.begin();i!=tries.end();i++)
					if(dDOT(test_tri.norm,i->norm)>0.f)
													ret+=dSortedTriBox(i->side0,i->side1,i->norm,
																	i->v0,i->v1,i->v2,i->dist,
																	o1,o2,flags,
																	CONTACT(contact, ret * skip),
																	skip);
 
 
return ret;
}


int dcTriListCollider::CollideBox(dxGeom* Box, int Flags, dContactGeom* Contacts, int Stride){

//	dcOBBTreeCollider& Collider = OBBCollider;



	/* Get box */
	Fvector AABB;
	Fvector BoxExtents;
	
	Fvector* BoxCenter=(Fvector*)const_cast<dReal*>(dGeomGetPosition(Box));
	dVector3 BoxSides;///=(dReal*)BoxExtents;
	
	
	dGeomBoxGetLengths(Box, BoxSides);
	memcpy( &BoxExtents,&BoxSides,sizeof(Fvector));

	dReal* R=const_cast<dReal*>(dGeomGetRotation(Box));

	AABB.x=(dFabs(BoxSides[0]*R[0])+dFabs(BoxSides[1]*R[4])+dFabs(BoxSides[2]*R[8]))/2.f+1.f*EPS_L;
	AABB.y=(dFabs(BoxSides[0]*R[1])+dFabs(BoxSides[1]*R[5])+dFabs(BoxSides[2]*R[9]))/2.f+1000.f*EPS_L;
	AABB.z=(dFabs(BoxSides[0]*R[2])+dFabs(BoxSides[1]*R[6])+dFabs(BoxSides[2]*R[10]))/2.f+1.f*EPS_L;

	        //
        XRC.box_options                (0);
        XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),*BoxCenter,AABB);

        // 
        int count                                       =XRC.r_count   ();
        CDB::RESULT*    R_begin                         = XRC.r_begin();
        CDB::RESULT*    R_end                           = XRC.r_end();
        CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
int OutTriCount = 0;

return dSortTriCollide (Box,
						Geometry,
						3,
						CONTACT(Contacts, OutTriCount * Stride),   Stride,
						R_begin,R_end,T_array
				);
		

        for (CDB::RESULT* Res=R_begin; Res!=R_end; Res++)
        {
                CDB::TRI* T = T_array + Res->id;
                //
               // T->verts[0];
               // T->verts[1];
               // T->verts[2];
              
                // 
                if(	true
					//CDB::TestBBoxTri(RM,*BoxCenter,BoxExtents,T->verts,false)
					)
                {
///reinterpret_cast<dReal*> (&(*(T->verts))[1])
				OutTriCount+=dTriBox ((dReal*)T->verts[0],(dReal*)T->verts[1],(dReal*)T->verts[2],
								  Box,
								  Geometry,
								  3,
								  CONTACT(Contacts, OutTriCount * Stride),   Stride);



                }
        }

			return OutTriCount;
}

	



int dcTriListCollider::CollideSphere(dxGeom* Sphere, int Flags, dContactGeom* Contacts, int Stride){


	/* Get sphere */
	
	const dcVector3 SphereCenter(dGeomGetPosition(Sphere));

	const float SphereRadius = dGeomSphereGetRadius(Sphere);

	Fvector AABB;
	Fvector SphereCenterF;
	memcpy(&SphereCenterF,&SphereCenter,sizeof(Fvector));

	
	/* Make AABB */
	AABB.x=SphereRadius;//*2.f;
	AABB.y=SphereRadius;//*2.f;
	AABB.z=SphereRadius;//*2.f;

	

	/* Retrieve data */

		/* Creating minimum contacts */

		int OutTriCount = 0;
		//UINT TriangleIDCount; // Num of tries

       XRC.box_options                (0);
	   XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),SphereCenterF,AABB);

        // 
        int count                                       =XRC.r_count   ();
        CDB::RESULT*    R_begin                         = XRC.r_begin();
        CDB::RESULT*    R_end                           = XRC.r_end();
        CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();


	       for (CDB::RESULT* Res=R_begin; Res!=R_end; Res++)
        {
                CDB::TRI* T = T_array + Res->id;
                //
               // T->verts[0];
               // T->verts[1];
               // T->verts[2];
    

      
			const dcVector3& v0 = (dcVector3&)(*T->verts[0]);

			const dcVector3& v1 = (dcVector3&)(*T->verts[1]);

			const dcVector3& v2 = (dcVector3&)(*T->verts[2]);



			dcPlane TriPlane(v0, v1, v2);



			if (!TriPlane.Contains(SphereCenter)){

				continue;

			}

			
			dVector3 V[3]={
			{v0.x,v0.y,v0.z},
			{v1.x,v1.y,v1.z},
			{v2.x,v2.y,v2.z}
			};
			dVector3 norm;	
			bool isLC=false;
			bool isPC=false;
			float Depth;
			for(int i=0;i<3;i++){

			Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
											dGeomSphereGetRadius(Sphere),
											V[i],V[(i+1)%3],norm);
			
			if	(Depth>0.f) {

								 isLC=true;
								break;
							}
								 ;

			}
		if(!isLC)
			for(int i=0;i<3;i++){

			Depth=PointSphereTest(dGeomGetPosition(Sphere),
											dGeomSphereGetRadius(Sphere),
											V[i],norm);
			
			if	(Depth>0.f) {

								isPC=true;
								break;
							};

			}		
			
			dcVector3 ContactNormal=TriPlane.Normal;  ///&TriPlane.Normal

			dcVector3 ContactPos=SphereCenter - ContactNormal * SphereRadius;

			float ContactDepth= TriPlane.Distance - SphereCenter.DotProduct(TriPlane.Normal) + SphereRadius;
			if (ContactDepth >= 0){

					int Index;
					bool contains=true;
					{
					dcPlane Plane(v0, v1, v1 - TriPlane.Normal);
					if (!Plane.Contains(ContactPos))contains=false;
					}
					{
					dcPlane Plane(v1, v2, v2 - TriPlane.Normal);
					if (!Plane.Contains(ContactPos))contains=false;
					}
					{
					dcPlane Plane(v2, v0, v0 - TriPlane.Normal);
					if (!Plane.Contains(ContactPos))contains=false;
					}

				
			
			if(contains&&ContactDepth>Depth&&ContactDepth>0.f)
				{
				isLC=false;
				isPC=false;
				}
				
			if(isLC||isPC){
				ContactNormal=norm;
				ContactPos=SphereCenter-ContactNormal;
				ContactDepth=Depth;
				}
				
				
				dNormalize3(ContactNormal);
			
				
				if (contains||isPC||isLC){ 

						dContactGeom* Contact = CONTACT(Contacts, OutTriCount * Stride);

						((dcVector3&)Contact->normal) =-ContactNormal;// TriPlane.Normal;

						Contact->depth = ContactDepth;
						////////////////////
						((dcVector3&)(Contact->pos))=ContactPos;
						Contact->g1 = Geometry;
						Contact->g2 = Sphere;
						//////////////////////////////////
						OutTriCount++;

	

				}
		
			}

		}
		return OutTriCount;



	}




