// Do NOT build this file seperately. It is included in dTriList.cpp automatically.

//#define GENERATEBODIES
#include "..\..\cl_intersect.h"


dcTriListCollider::dcTriListCollider(dxGeom* Geometry)
{

	this->Geometry = Geometry;

	GeomData = (dxTriList*)dGeomGetClassData(Geometry);

	//memset(BoxContacts, 0, sizeof(BoxContacts));
}



dcTriListCollider::~dcTriListCollider(){

	//

}





int dCollideBP (const dxGeom* o1, const dxGeom* o2, int flags, dContactGeom *contact, int skip);	// ODE internal function



#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))



int dcTriListCollider::CollideBox(dxGeom* Box, int Flags, dContactGeom* Contacts, int Stride){

//	dcOBBTreeCollider& Collider = OBBCollider;



	/* Get box */

	Fvector BoxCenter,BoxExtents,AABB;
	dVector3 BoxSides;
	memcpy( &BoxCenter,dGeomGetPosition(Box),sizeof(Fvector));
	dGeomBoxGetLengths(Box, BoxSides);
	memcpy( &BoxExtents,&BoxSides,sizeof(Fvector));
	Fmatrix33 RM;
	const dReal* R=dGeomGetRotation(Box);
	memcpy( &RM.i,R+0,sizeof(Fvector));
	memcpy( &RM.j,R+4,sizeof(Fvector));
	memcpy( &RM.k,R+8,sizeof(Fvector));


	AABB.x=dFabs(BoxSides[0]*R[0])+dFabs(BoxSides[1]*R[4])+dFabs(BoxSides[2]*R[8]);
	AABB.y=dFabs(BoxSides[0]*R[1])+dFabs(BoxSides[1]*R[5])+dFabs(BoxSides[2]*R[9]);
	AABB.z=dFabs(BoxSides[0]*R[2])+dFabs(BoxSides[1]*R[6])+dFabs(BoxSides[2]*R[10]);

	        //
        XRC.box_options                (0);
        XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),BoxCenter,AABB);

        // 
        int count                                       =XRC.r_count   ();
        CDB::RESULT*    R_begin                         = XRC.r_begin();
        CDB::RESULT*    R_end                           = XRC.r_end();
        CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();

		int OutTriCount = 0;

        for (CDB::RESULT* Res=R_begin; Res!=R_end; Res++)
        {
                CDB::TRI* T = T_array + Res->id;
                //
               // T->verts[0];
               // T->verts[1];
               // T->verts[2];
              
                // 
                if(CDB::TestBBoxTri(RM,BoxCenter,BoxExtents,T->verts,false))
                {

				OutTriCount+=dTriBox (reinterpret_cast<dReal*> (&(*(T->verts))[0]),reinterpret_cast<dReal*> (&(*(T->verts))[1]),reinterpret_cast<dReal*> (&(*(T->verts))[2]),
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
	AABB.x=SphereRadius*2.f;
	AABB.y=SphereRadius*2.f;
	AABB.z=SphereRadius*2.f;

	

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
				
				
				
				
				if (contains||isPC||isLC){ 

					for (Index = 0; Index < OutTriCount; Index++){

						dContactGeom* RefContact = CONTACT(Contacts, Index * Stride);

						const dcVector3& RefNormal = (dcVector3&)RefContact->normal;



						if (TriPlane.Normal.DotProduct(RefNormal) > 0.9999f){	// Coplanitary test//?????//@slipch I set 0.9999 instead 0.9 I not completely understand when this works.

							RefContact->depth = dcMAX((float)RefContact->depth, ContactDepth);

							break;

						}

					}



					if (Index == OutTriCount||isPC||isLC){

						dContactGeom* Contact = CONTACT(Contacts, OutTriCount * Stride);

						((dcVector3&)Contact->normal) = TriPlane.Normal;

						Contact->depth = ContactDepth;

						OutTriCount++;

					}

				}
				if(isPC) break;

			}

		}



		if (OutTriCount != 0){

			dcVector3 OutNormal(0, 0, 0);

		

			/* Combining contacts */

			for (int i = 0; i < OutTriCount; i++){

				dContactGeom* Contact = CONTACT(Contacts, i * Stride);



				OutNormal += ((dcVector3&)Contact->normal) * Contact->depth;

			}



			const float DepthSq = OutNormal.MagnitudeSq();



			/* Generating final contact */

			if (DepthSq > 0.0f){

				const float Depth = sqrtf(DepthSq);



				OutNormal /= Depth;	// Normalizing

			
				dContactGeom& OutContact = *Contacts;

				((dcVector3&)OutContact.pos) = SphereCenter - OutNormal * SphereRadius;

				((dcVector3&)OutContact.normal) =OutNormal * -1;

				OutContact.depth = Depth;



				OutContact.g1 = Geometry;

				OutContact.g2 = Sphere;




				return 1;

			}

			else return 0;

		}

		else return 0;

	}




