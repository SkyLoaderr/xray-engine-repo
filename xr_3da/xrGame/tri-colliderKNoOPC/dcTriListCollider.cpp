// Do NOT build this file seperately. It is included in dTriList.cpp automatically.


#include "../../cl_intersect.h"


dcTriListCollider::dcTriListCollider(dxGeom* Geometry)
{
	this->Geometry = Geometry;
	GeomData = (dxTriList*)dGeomGetClassData(Geometry);

#ifdef DEBUG
	Device.seqRender.Add	(this,REG_PRIORITY_LOW-999);
	BoxCenter = 0;
#endif
}

dcTriListCollider::~dcTriListCollider()
{
#ifdef DEBUG
	Device.seqRender.Remove	(this);
#endif
}





int dCollideBP (const dxGeom* o1, const dxGeom* o2, int flags, dContactGeom *contact, int skip);	// ODE internal function



#define CONTACT(Ptr, Stride) ((dContactGeom*) (((byte*)Ptr) + (Stride)))

#define SURFACE(Ptr, Stride) ((dSurfaceParameters*) (((byte*)Ptr) + (Stride-sizeof(dSurfaceParameters))))


static 	xr_vector<Triangle> pos_tries;
//static 	xr_vector<Triangle> neg_tries;


extern "C" int dSortTriBoxCollide (
								   dxGeom *o1,		dxGeom *o2,
								   int flags,		dContactGeom *contact,	int skip,
								   CDB::RESULT*		R_begin,
								   CDB::RESULT*		R_end ,
								   CDB::TRI*		T_array,
								   Fvector			AABB
								   )
{
	const Fvector*	V_array			= Level().ObjectSpace.GetStaticVerts();

	//	Log("in dSortTriBoxCollide");
	//Msg("%f",dInfinity);
	int			ret	=	0;
	Triangle	tri;
	//bool pushing_b_neg_reset=false,pushing_neg_reset=false;
	dxGeomUserData* data=dGeomGetUserData(o1);
	Triangle* neg_tri=&(data->neg_tri);
	Triangle* b_neg_tri=&(data->b_neg_tri);
	dReal* last_pos=data->last_pos;

	bool* pushing_neg=&data->pushing_neg;
	bool* pushing_b_neg=&data->pushing_b_neg;
	pos_tries.clear	();
	//neg_tries.clear	();
	//pos_dist=dInfinity,
	dReal neg_depth=dInfinity,b_neg_depth=dInfinity;
	//dReal max_proj=-dInfinity,proj;
	const dReal* p=dGeomGetPosition(o1);
	UINT b_count=0;
	//dVector3 pos_vect={last_pos[0]-p[0],last_pos[1]-p[1],last_pos[2]-p[2]};


	dVector3 hside;
	dGeomBoxGetLengths(o1,hside);
	hside[0]/=2.f;hside[2]/=2.f;hside[2]/=2.f;
	const dReal *R = dGeomGetRotation(o1);

	//if(last_pos[0]==dInfinity) Memory.mem_copy(last_pos,p,sizeof(dVector3));

	if(*pushing_neg){
		dReal sidePr=
			dFabs(dDOT14(neg_tri->norm,R+0)*hside[0])+
			dFabs(dDOT14(neg_tri->norm,R+1)*hside[1])+
			dFabs(dDOT14(neg_tri->norm,R+2)*hside[2]);
		neg_tri->dist=dDOT(p,neg_tri->norm)-neg_tri->pos;
		neg_tri->depth=sidePr-neg_tri->dist;

		if(neg_tri->dist<0.f)
			neg_depth=neg_tri->depth;
		else
		{
			*pushing_neg=false;

		}
	}

	if(*pushing_b_neg){
		dReal sidePr=
			dFabs(dDOT14(b_neg_tri->norm,R+0)*hside[0])+
			dFabs(dDOT14(b_neg_tri->norm,R+1)*hside[1])+
			dFabs(dDOT14(b_neg_tri->norm,R+2)*hside[2]);
		b_neg_tri->dist=dDOT(p,b_neg_tri->norm)-b_neg_tri->pos;
		b_neg_tri->depth=sidePr-b_neg_tri->dist;

		if(b_neg_tri->dist<0.f)
			b_neg_depth=b_neg_tri->depth;
		else{
			*pushing_b_neg=false;

		}
	}



	/*
	if(pushing_neg){
	ret+=dSortedTriBox(neg_tri->side0,neg_tri->side1,neg_tri->norm,
	neg_tri->v0,neg_tri->v1,neg_tri->v2,neg_tri->dist,
	o1,o2,flags,
	CONTACT(contact, ret * skip),
	skip);	
	return ret;
	}

	if(pushing_b_neg){
	ret+=dSortedTriBox(b_neg_tri->side0,b_neg_tri->side1,b_neg_tri->norm,
	b_neg_tri->v0,b_neg_tri->v1,b_neg_tri->v2,b_neg_tri->dist,
	o1,o2,flags,
	CONTACT(contact, ret * skip),
	skip);	
	return ret;
	}

	*/

	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	{
		CDB::TRI* T = T_array + Res->id;

		tri.side0[0]=Res->verts[1].x-Res->verts[0].x;
		tri.side0[1]=Res->verts[1].y-Res->verts[0].y;
		tri.side0[2]=Res->verts[1].z-Res->verts[0].z;

		tri.side1[0]=Res->verts[2].x-Res->verts[1].x;
		tri.side1[1]=Res->verts[2].y-Res->verts[1].y;
		tri.side1[2]=Res->verts[2].z-Res->verts[1].z;

		tri.T=T;
		dCROSS(tri.norm,=,tri.side0,tri.side1);
		dNormalize3(tri.norm);
		dReal sidePr=
			dFabs(dDOT14(tri.norm,R+0)*hside[0])+
			dFabs(dDOT14(tri.norm,R+1)*hside[1])+
			dFabs(dDOT14(tri.norm,R+2)*hside[2]);

		tri.pos=dDOT((dReal*)Res->verts + 0,tri.norm);
		tri.dist=dDOT(p,tri.norm)-tri.pos;
		tri.depth=sidePr-tri.dist;
		Point vertices[3]={Point((dReal*)Res->verts+0),Point((dReal*)Res->verts + 1),Point((dReal*)Res->verts+2)};
		if(tri.dist<0.f){
			if((!(dDOT(last_pos,tri.norm)-tri.pos<0.f))||*pushing_neg||*pushing_b_neg)
				if(__aabb_tri(Point(p),Point((float*)&AABB),vertices))
				{
					if(TriContainPoint((dReal*)Res->verts+0,(dReal*)Res->verts+1,(dReal*)Res->verts+2,
						tri.norm,tri.side0,
						tri.side1,p)

						){
							if(neg_depth>tri.depth)
							{
								neg_depth=tri.depth;
								(*neg_tri)=tri;
								//		pushing_neg_reset=true;
							}


						}
					else{
						++b_count;
						if(b_neg_depth>tri.depth){
							b_neg_depth=tri.depth;
							(*b_neg_tri)=tri;
							//	pushing_b_neg_reset=true;
						}
					}
				}
		}
		else{
			pos_tries.push_back(tri);
			//if(tri.dist<pos_dist) pos_dist=tri.dist;
		}
	}


	xr_vector<Triangle>::iterator i;



	//(*pushing_neg)=(*pushing_neg)&&(!ret);



	if(neg_depth<dInfinity&&ret==0){
		bool include = true;
		/*
		for(i=pos_tries.begin();pos_tries.end() != i;++i){
		if(TriContainPoint(
		//i->v0,i->v1,i->v2,
		(dReal*)i->T->verts[0],(dReal*)i->T->verts[1],(dReal*)i->T->verts[2],
		i->norm,i->side0,
		i->side1,p))
		if((dDOT(neg_tri->norm,(dReal*)i->T->verts[0])-neg_tri->pos)<0.f||
		(dDOT(neg_tri->norm,(dReal*)i->T->verts[1])-neg_tri->pos)<0.f||
		(dDOT(neg_tri->norm,(dReal*)i->T->verts[2])-neg_tri->pos)<0.f
		){
		include=false;
		break;
		}
		};
		*/		
		if(include){		
			ret+=dSortedTriBox(neg_tri->side0,neg_tri->side1,neg_tri->norm,
				//neg_tri->v0,neg_tri->v1,neg_tri->v2,
				neg_tri->T,
				neg_tri->dist,
				o1,o2,flags,
				CONTACT(contact, ret * skip),
				skip);	
			*pushing_neg=!!ret;
		}

	}

	//(*pushing_b_neg)=(*pushing_b_neg)&&(!ret);
	//if(ret==0)
	for(i=pos_tries.begin();pos_tries.end() != i;++i){

		ret+=dTriBox (
			//i->v0,i->v1,i->v2,
			i->T,
			o1,
			o2,
			3,
			CONTACT(contact, ret * skip),   skip);

		//for(int i=0;i<add;++i)

		//ret+=add;
	}
	//((b_count>1)||(*pushing_b_neg))&&

	if(b_neg_depth<dInfinity&&ret==0){
		bool include = true;

		for(i=pos_tries.begin();pos_tries.end() != i;++i){
			if((((dDOT(b_neg_tri->norm,(dReal*)&V_array[i->T->verts[0]])-b_neg_tri->pos)<0.f)||
				((dDOT(b_neg_tri->norm,(dReal*)&V_array[i->T->verts[1]])-b_neg_tri->pos)<0.f)||
				((dDOT(b_neg_tri->norm,(dReal*)&V_array[i->T->verts[2]])-b_neg_tri->pos)<0.f))
				){
					include=false;
					break;
				}
		};

		if(include)	
		{	
			ret+=dSortedTriBox(b_neg_tri->side0,b_neg_tri->side1,b_neg_tri->norm,
				//b_neg_tri->v0,b_neg_tri->v1,b_neg_tri->v2,
				b_neg_tri->T,
				b_neg_tri->dist,
				o1,o2,flags,
				CONTACT(contact, ret * skip),
				skip);	
			*pushing_b_neg=!!ret;
		}

	}
	Memory.mem_copy(last_pos,p,sizeof(dVector3));
	return ret;
}

int dcTriListCollider::CollideBox(dxGeom* Box, int Flags, dContactGeom* Contacts, int Stride){

	//	dcOBBTreeCollider& Collider = OBBCollider;



	/* Get box */
#ifndef DEBUG
	Fvector AABB;
	Fvector* BoxCenter=0;
#endif
	Fvector BoxExtents;

	BoxCenter=(Fvector*)const_cast<dReal*>(dGeomGetPosition(Box));
	//dVector3 BoxSides;///=(dReal*)BoxExtents;


	//dGeomBoxGetLengths(Box, BoxSides);


	dVector3 BoxSides;
	dGeomBoxGetLengths(Box,BoxSides);
	Memory.mem_copy( &BoxExtents,&BoxSides,sizeof(Fvector));

	dReal* R=const_cast<dReal*>(dGeomGetRotation(Box));

	AABB.x=(dFabs(BoxSides[0]*R[0])+dFabs(BoxSides[1]*R[1])+dFabs(BoxSides[2]*R[2]))/2.f+100.f*EPS_L;
	AABB.y=(dFabs(BoxSides[0]*R[4])+dFabs(BoxSides[1]*R[5])+dFabs(BoxSides[2]*R[6]))/2.f+100.f*EPS_L;
	AABB.z=(dFabs(BoxSides[0]*R[8])+dFabs(BoxSides[1]*R[9])+dFabs(BoxSides[2]*R[10]))/2.f+100.f*EPS_L;
	dBodyID box_body=dGeomGetBody(Box);
	if(box_body){
		const dReal*velocity=dBodyGetLinearVel(box_body);
		AABB.x+=dFabs(velocity[0])*0.02f;
		AABB.y+=dFabs(velocity[1])*0.02f;
		AABB.z+=dFabs(velocity[2])*0.02f;
	}
	//
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),*BoxCenter,AABB);

	// 
	//		int count                                       =XRC.r_count   ();
	CDB::RESULT*    R_begin                         = XRC.r_begin();
	CDB::RESULT*    R_end                           = XRC.r_end();
	CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
	int OutTriCount = 0;

	return dSortTriBoxCollide (Box,
		Geometry,
		3,
		CONTACT(Contacts, OutTriCount * Stride),   Stride,
		R_begin,R_end,T_array,AABB
		);

	/*
	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
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
	*/
}



int dcTriListCollider::CollideCylinder(dxGeom* Cylinder, int Flags, dContactGeom* Contacts, int Stride){


	Fvector* CylinderCenter;
	Fvector AABB;
	dReal CylinderRadius,CylinderLength;
	CylinderCenter=(Fvector*)const_cast<dReal*>(dGeomGetPosition(Cylinder));


	dGeomCylinderGetParams (Cylinder, &CylinderRadius,&CylinderLength);



	dReal* R=const_cast<dReal*>(dGeomGetRotation(Cylinder));

	AABB.x =  REAL(0.5) * dFabs (R[1] * CylinderLength) + (_sqrt(R[0]*R[0]+R[2]*R[2]) * CylinderRadius);

	AABB.y =  REAL(0.5) * dFabs (R[5] * CylinderLength) + (_sqrt(R[4]*R[4]+R[6]*R[6]) * CylinderRadius);

	AABB.z =  REAL(0.5) * dFabs (R[9] * CylinderLength) + (_sqrt(R[8]*R[8]+R[10]*R[10]) * CylinderRadius);

	const dReal*velocity=dBodyGetLinearVel(dGeomGetBody(Cylinder));
	AABB.x+=dFabs(velocity[0])*0.01f;
	AABB.y+=dFabs(velocity[1])*0.01f;
	AABB.z+=dFabs(velocity[2])*0.01f;


	//
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),* CylinderCenter,AABB);

	// 
	//	int count                                       =XRC.r_count   ();
	CDB::RESULT*    R_begin                         = XRC.r_begin();
	CDB::RESULT*    R_end                           = XRC.r_end();
	CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
	int OutTriCount = 0;

	///@slipch

	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	{
		CDB::TRI* T = T_array + Res->id;
		OutTriCount+=dTriCyl (
			//(dReal*)T->verts[0],(dReal*)T->verts[1],(dReal*)T->verts[2],
			T,
			Cylinder,
			Geometry,
			3,
			CONTACT(Contacts, OutTriCount * Stride),   Stride);
	}
	return OutTriCount;



}
///end @slipch

int dTriSphere(//const dReal* v0,const dReal* v1,const dReal* v2,
			   CDB::TRI* T,
			   dxGeom* Sphere,dxGeom* Geometry, int Flags, 
			   dContactGeom* Contacts)
{
	const Fvector*	V_array			=	Level().ObjectSpace.GetStaticVerts();
	const dReal*	SphereCenter	=	dGeomGetPosition	(Sphere);
	const float		SphereRadius	=	dGeomSphereGetRadius(Sphere);
	const dReal* v0	=(dReal*)&V_array[T->verts[0]];
	const dReal* v1	=(dReal*)&V_array[T->verts[1]];
	const dReal* v2	=(dReal*)&V_array[T->verts[2]];
	//dVector3 triSideAx0={V[1][0]-V[0][0],V[1][1]-V[0][1],V[1][2]-V[0][2]};
	//	dVector3 triSideAx1={V[2][0]-V[1][0],V[2][1]-V[1][1],V[2][2]-V[1][2]};
	//		dVector3 triSideAx2={V[0][0]-V[2][0],V[0][1]-V[2][1],V[0][2]-V[2][2]};
	dVector3 triSideAx0={v1[0]-v0[0],v1[1]-v0[1],v1[2]-v0[2]};
	dVector3 triSideAx1={v2[0]-v1[0],v2[1]-v1[1],v2[2]-v1[2]};
	dVector3 triAx;
	dCROSS(triAx,=,triSideAx0,triSideAx1);

	if(!TriPlaneContainPoint(triAx,v0,SphereCenter)) return 0;

	dVector3 norm;	
	bool isLC=false;
	bool isPC=false;
	float Depth=0;
	/*
	for(int i=0;i<3;++i){

	Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
	dGeomSphereGetRadius(Sphere),
	V[i],V[(i+1)%3],norm);

	if	(Depth>0.f) {

	isLC=true;
	break;
	}
	;

	}
	*/
	Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
		dGeomSphereGetRadius(Sphere),
		v0,v1,norm);
	isLC=Depth>0.f;
	if(!isLC){ Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
		dGeomSphereGetRadius(Sphere),
		v1,v2,norm);
	isLC=Depth>0.f;
	}
	if(!isLC){ Depth=FragmentonSphereTest(dGeomGetPosition(Sphere),
		dGeomSphereGetRadius(Sphere),
		v2,v0,norm);
	}
	/*
	if(!isLC)
	for(int i=0;i<3;++i){

	Depth=PointSphereTest(dGeomGetPosition(Sphere),
	dGeomSphereGetRadius(Sphere),
	V[i],norm);

	if	(Depth>0.f) {

	isPC=true;
	break;
	};

	}		

	*/
	if(!isLC){
		Depth=PointSphereTest(dGeomGetPosition(Sphere),
			dGeomSphereGetRadius(Sphere),v0,norm);
		isPC=Depth>0.f;
		if(!isPC){Depth=PointSphereTest(dGeomGetPosition(Sphere),
			dGeomSphereGetRadius(Sphere),v1,norm);
		isPC=Depth>0.f;
		}

		if(!isPC){Depth=PointSphereTest(dGeomGetPosition(Sphere),
			dGeomSphereGetRadius(Sphere),v2,norm);
		isPC=Depth>0.f;
		}

	}

	dNormalize3(triAx);
	dVector3 ContactNormal={triAx[0],triAx[1],triAx[2]};
	dVector3 ContactPos={SphereCenter[0]-triAx[0]* SphereRadius,SphereCenter[1]-triAx[1]* SphereRadius,SphereCenter[2]-triAx[2]* SphereRadius};

	float ContactDepth= dDOT(triAx,v0) - dDOT(SphereCenter,triAx) + SphereRadius;
	if (ContactDepth >= 0){


		bool contains=TriContainPoint(v0,v1,v2,ContactPos);

		if(contains&&ContactDepth>Depth&&ContactDepth>0.f)
		{
			isLC=false;
			isPC=false;
		}

		if(isLC||isPC){

			ContactNormal[0]=norm[0];
			ContactNormal[1]=norm[1];
			ContactNormal[2]=norm[2];
			ContactPos[0]=SphereCenter[0]-ContactNormal[0];
			ContactPos[1]=SphereCenter[1]-ContactNormal[1];
			ContactPos[2]=SphereCenter[2]-ContactNormal[2];

			ContactDepth=Depth;
		}


		dNormalize3(ContactNormal);


		if (contains||isPC||isLC){

			//	dContactGeom* Contact = CONTACT(Contacts, Skip);

			Contacts->normal[0] =-ContactNormal[0];
			Contacts->normal[1] =-ContactNormal[1];
			Contacts->normal[2] =-ContactNormal[2];
			Contacts->depth = ContactDepth;
			////////////////////

			Contacts->pos[0]=ContactPos[0];
			Contacts->pos[1]=ContactPos[1];
			Contacts->pos[2]=ContactPos[2];
			Contacts->g1 = Geometry;
			Contacts->g2 = Sphere;
			((dxGeomUserData*)dGeomGetData(Sphere))->tri_material=T->material;
			SURFACE(Contacts,0)->mu=GMLib.GetMaterial(T->material)->fPHFriction;
			SURFACE(Contacts,0)->bounce=GMLib.GetMaterial(T->material)->fPHBouncing;
			SURFACE(Contacts,0)->bounce_vel=GMLib.GetMaterial(T->material)->fPHBounceStartVelocity;
			SURFACE(Contacts,0)->soft_cfm=GMLib.GetMaterial(T->material)->fPHSpring;
			SURFACE(Contacts,0)->soft_erp=GMLib.GetMaterial(T->material)->fPHDamping;
			SURFACE(Contacts,0)->mode=GMLib.GetMaterial(T->material)->Flags.get();
			//////////////////////////////////
			//	++OutTriCount;
			return 1;


		}

	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////
int dSortedTriSphere(//const dReal* v0,const dReal* v1,const dReal* v2,
					 CDB::TRI* T,
					 const dReal* triAx,
					 dxGeom* Sphere,dxGeom* Geometry, int Flags, dContactGeom* Contacts){

						 const dReal* v0=(dReal*)T->verts[0];
						 //const dReal* v1=(dReal*)T->verts[1];
						 //const dReal* v2=(dReal*)T->verts[2];
						 const dReal* SphereCenter=dGeomGetPosition(Sphere);
						 const float SphereRadius = dGeomSphereGetRadius(Sphere);



						 //	dNormalize3(triAx);
						 dVector3 ContactNormal={triAx[0],triAx[1],triAx[2]};
						 dVector3 ContactPos={SphereCenter[0]-triAx[0]* SphereRadius,SphereCenter[1]-triAx[1]* SphereRadius,SphereCenter[2]-triAx[2]* SphereRadius};

						 float ContactDepth= dDOT(triAx,v0) - dDOT(SphereCenter,triAx) + SphereRadius;
						 if (ContactDepth >= 0){






							 //dContactGeom* Contact = CONTACT(Contacts, Skip);

							 Contacts->normal[0] =-ContactNormal[0];
							 Contacts->normal[1] =-ContactNormal[1];
							 Contacts->normal[2] =-ContactNormal[2];
							 Contacts->depth = ContactDepth;
							 ////////////////////

							 Contacts->pos[0]=ContactPos[0];
							 Contacts->pos[1]=ContactPos[1];
							 Contacts->pos[2]=ContactPos[2];
							 Contacts->g1 = Geometry;
							 Contacts->g2 = Sphere;
							 ((dxGeomUserData*)dGeomGetData(Sphere))->tri_material=T->material;
							 SURFACE(Contacts,0)->mu=GMLib.GetMaterial(T->material)->fPHFriction;
							 SURFACE(Contacts,0)->bounce=GMLib.GetMaterial(T->material)->fPHBouncing;
							 SURFACE(Contacts,0)->bounce_vel=GMLib.GetMaterial(T->material)->fPHBounceStartVelocity;
							 SURFACE(Contacts,0)->soft_cfm=GMLib.GetMaterial(T->material)->fPHSpring;
							 SURFACE(Contacts,0)->soft_erp=GMLib.GetMaterial(T->material)->fPHDamping;
							 //SURFACE(Contacts,0)->mode=GMLib.GetMaterial(T->material)->Flags.is(SGameMtl::flClimbable);
							 SURFACE(Contacts,0)->mode=GMLib.GetMaterial(T->material)->Flags.get();
							 //////////////////////////////////
							 //	++OutTriCount;
							 return 1;




						 }

						 return 0;
					 }


					 ////////////////////////////////////////////////////////////////////////////
					 extern "C" int dSortTriSphereCollide (
						 dxGeom *o1, dxGeom *o2,
						 int flags, dContactGeom *contact, int skip,
						 CDB::RESULT*    R_begin,
						 CDB::RESULT*    R_end ,
						 CDB::TRI*       T_array,
						 Fvector AABB
						 )
					 {
						 //	Log("in dSortTriBoxCollide");
						 //Msg("%f",dInfinity);
						 int ret=0;
						 Triangle tri;
						 //bool pushing_b_neg_reset=false,pushing_neg_reset=false;
						 dxGeomUserData* data=dGeomGetUserData(o1);
						 Triangle* neg_tri=&(data->neg_tri);
						 Triangle* b_neg_tri=&(data->b_neg_tri);
						 dReal* last_pos=data->last_pos;

						 bool* pushing_neg=&data->pushing_neg;
						 bool* pushing_b_neg=&data->pushing_b_neg;
						 pos_tries.clear	();
						 //neg_tries.clear	();
						 //pos_dist=dInfinity,
						 dReal neg_depth=dInfinity,b_neg_depth=dInfinity;
						 //dReal max_proj=-dInfinity,proj;
						 const dReal* p=dGeomGetPosition(o1);
						 UINT b_count=0;
						 //dVector3 pos_vect={last_pos[0]-p[0],last_pos[1]-p[1],last_pos[2]-p[2]};


						 dReal sidePr=dGeomSphereGetRadius(o1);

						 //if(last_pos[0]==dInfinity) Memory.mem_copy(last_pos,p,sizeof(dVector3));

						 if(*pushing_neg){
							 neg_tri->dist=dDOT(p,neg_tri->norm)-neg_tri->pos;
							 neg_tri->depth=sidePr-neg_tri->dist;

							 if(neg_tri->dist<0.f)
								 neg_depth=neg_tri->depth;
							 else
							 {
								 *pushing_neg=false;

							 }
						 }

						 if(*pushing_b_neg){

							 b_neg_tri->dist=dDOT(p,b_neg_tri->norm)-b_neg_tri->pos;
							 b_neg_tri->depth=sidePr-b_neg_tri->dist;

							 if(b_neg_tri->dist<0.f)
								 b_neg_depth=b_neg_tri->depth;
							 else{
								 *pushing_b_neg=false;

							 }
						 }


						 for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
						 {
							 CDB::TRI* T = T_array + Res->id;
							 tri.side0[0]=Res->verts[1].x-Res->verts[0].x;
							 tri.side0[1]=Res->verts[1].y-Res->verts[0].y;
							 tri.side0[2]=Res->verts[1].z-Res->verts[0].z;

							 tri.side1[0]=Res->verts[2].x-Res->verts[1].x;
							 tri.side1[1]=Res->verts[2].y-Res->verts[1].y;
							 tri.side1[2]=Res->verts[2].z-Res->verts[1].z;
							 tri.T		=T;
							 dCROSS		(tri.norm,=,tri.side0,tri.side1);
							 dNormalize3	(tri.norm);


							 tri.pos=dDOT((dReal*)Res->verts + 0,tri.norm);
							 tri.dist=dDOT(p,tri.norm)-tri.pos;
							 tri.depth=sidePr-tri.dist;
							 Point vertices[3]={Point((dReal*)Res->verts + 0),Point((dReal*)Res->verts + 1),Point((dReal*)Res->verts + 2)};
							 if(tri.dist<0.f){
								 if( (!(dDOT(last_pos,tri.norm)-tri.pos<0.f)&& -dInfinity != last_pos[0]) ||*pushing_neg||*pushing_b_neg)
									 if(__aabb_tri(Point(p),Point((float*)&AABB),vertices))
									 {
										 if(TriContainPoint((dReal*)Res->verts + 0,(dReal*)Res->verts + 1,(dReal*)Res->verts + 2,
											 tri.norm,tri.side0,
											 tri.side1,p)

											 ){
												 if(neg_depth>tri.depth)
												 {
													 neg_depth=tri.depth;
													 (*neg_tri)=tri;
													 //		pushing_neg_reset=true;
												 }


											 }
										 else{
											 ++b_count;
											 if(b_neg_depth>tri.depth){
												 b_neg_depth=tri.depth;
												 (*b_neg_tri)=tri;
												 //	pushing_b_neg_reset=true;
											 }
										 }
									 }
							 }
							 else{
								 pos_tries.push_back(tri);
								 //if(tri.dist<pos_dist) pos_dist=tri.dist;
							 }
						 }


						 xr_vector<Triangle>::iterator i;



						 //(*pushing_neg)=(*pushing_neg)&&(!ret);



						 if(neg_depth<dInfinity&&ret==0){
							 bool include = true;
							 /*
							 for(i=pos_tries.begin();pos_tries.end() != i;++i){
							 if(TriContainPoint((dReal*)i->T->verts[0],(dReal*)i->T->verts[1],(dReal*)i->T->verts[2],
							 i->norm,i->side0,
							 i->side1,p))
							 if((dDOT(neg_tri->norm,(dReal*)i->T->verts[0])-neg_tri->pos)<0.f||
							 (dDOT(neg_tri->norm,(dReal*)i->T->verts[1])-neg_tri->pos)<0.f||
							 (dDOT(neg_tri->norm,(dReal*)i->T->verts[2])-neg_tri->pos)<0.f
							 ){
							 include=false;
							 break;
							 }
							 };
							 */	
							 if(include){
								 ret+=dSortedTriSphere(
									 //neg_tri->v0,neg_tri->v1,neg_tri->v2,
									 neg_tri->T,
									 neg_tri->norm,
									 o1,o2,flags,
									 CONTACT(contact, ret * skip));	


								 *pushing_neg=!!ret;
							 }

						 }

						 //(*pushing_b_neg)=(*pushing_b_neg)&&(!ret);
						 //if(ret==0)
						 for(i=pos_tries.begin();pos_tries.end()!=i;++i)

							 ret+=dTriSphere (
							 //i->v0,i->v1,i->v2,
							 i->T,
							 o1,
							 o2,
							 3,
							 CONTACT(contact, ret * skip));

						 //((b_count>1)||(*pushing_b_neg))&&
						 if(b_neg_depth<dInfinity&&ret==0){
							 bool include = true;
							 for(i=pos_tries.begin();pos_tries.end()!=i;++i){
								 if((((dDOT(b_neg_tri->norm,(dReal*)i->T->verts[0])-b_neg_tri->pos)<0.f)||
									 ((dDOT(b_neg_tri->norm,(dReal*)i->T->verts[1])-b_neg_tri->pos)<0.f)||
									 ((dDOT(b_neg_tri->norm,(dReal*)i->T->verts[2])-b_neg_tri->pos)<0.f))
									 ){
										 include=false;
										 break;
									 }
							 };

							 if(include)	{

								 ret+=dSortedTriSphere(
									 //b_neg_tri->v0,b_neg_tri->v1,b_neg_tri->v2,
									 b_neg_tri->T,
									 b_neg_tri->norm,
									 o1,o2,flags,
									 CONTACT(contact, ret * skip));	
								 *pushing_b_neg=!!ret;
							 }

						 }
						 Memory.mem_copy(last_pos,p,sizeof(dVector3));
						 return ret;
					 }
					 ///////////////////////////////////////////////////////////////////////////
					 int dcTriListCollider::CollideSphere(dxGeom* Sphere, int Flags, dContactGeom* Contacts, int Stride){


						 // Get sphere 


						 const dReal* SphereCenter=dGeomGetPosition(Sphere);

						 const float SphereRadius = dGeomSphereGetRadius(Sphere);

						 Fvector AABB;
						 Fvector SphereCenterF;

						 Memory.mem_copy(&SphereCenterF,SphereCenter,sizeof(Fvector));


						 // Make AABB 
						 AABB.x=SphereRadius;
						 AABB.y=SphereRadius;
						 AABB.z=SphereRadius;

						 const dReal*velocity=dBodyGetLinearVel(dGeomGetBody(Sphere));
						 AABB.x+=dFabs(velocity[0])*0.02f;
						 AABB.y+=dFabs(velocity[1])*0.02f;
						 AABB.z+=dFabs(velocity[2])*0.02f;

						 // Retrieve data 

						 // Creating minimum contacts 


						 //UINT TriangleIDCount; // Num of tries

						 XRC.box_options                (0);
						 XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),SphereCenterF,AABB);

						 // 
						 //	int count                                       = (int)XRC.r_count   ();
						 //	++count;
						 //	--count;
						 CDB::RESULT*    R_begin                         = XRC.r_begin();
						 CDB::RESULT*    R_end                           = XRC.r_end();
						 CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();
						 return dSortTriSphereCollide(Sphere,Geometry,Flags,Contacts,Stride,R_begin,R_end,T_array,AABB);
						 /*
						 for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
						 {
						 CDB::TRI* T = T_array + Res->id;




						 OutTriCount+=dTriSphere((dReal*)(T->verts[0]),(dReal*)(T->verts[1]),(dReal*)(T->verts[2]),Sphere,Geometry,Flags,CONTACT(Contacts, OutTriCount * Stride));
						 }
						 return OutTriCount;


						 */

					 }


					 /*
					 int dcTriListCollider::CollideSphere(dxGeom* Sphere, int Flags, dContactGeom* Contacts, int Stride){


					 // Get sphere 

					 const dcVector3 SphereCenter(dGeomGetPosition(Sphere));

					 const float SphereRadius = dGeomSphereGetRadius(Sphere);

					 Fvector AABB;
					 Fvector SphereCenterF;
					 Memory.mem_copy(&SphereCenterF,&SphereCenter,sizeof(Fvector));


					 //Make AABB 
					 AABB.x=SphereRadius;//
					 AABB.y=SphereRadius;//
					 AABB.z=SphereRadius;//



					 // Retrieve data 

					 // Creating minimum contacts 

					 int OutTriCount = 0;
					 //UINT TriangleIDCount; // Num of tries

					 XRC.box_options                (0);
					 XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),SphereCenterF,AABB);

					 // 
					 //int count                                       =XRC.r_count   ();
					 CDB::RESULT*    R_begin                         = XRC.r_begin();
					 CDB::RESULT*    R_end                           = XRC.r_end();
					 CDB::TRI*       T_array                         = Level().ObjectSpace.GetStaticTris();


					 for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
					 {
					 CDB::TRI* T = T_array + Res->id;




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
					 float Depth=0;
					 for(int i=0;i<3;++i){

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
					 for(int i=0;i<3;++i){

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

					 //				int Index;
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
					 ++OutTriCount;



					 }

					 }

					 }
					 return OutTriCount;



					 }
					 */

#ifdef DEBUG
					 void dcTriListCollider::OnRender()
					 {/*
					  if (!bDebug && BoxCenter){
					  Fvector C;
					  C.set(0,0,0);
					  RCache.dbg_DrawAABB(*BoxCenter,AABB.x,AABB.y,AABB.z,D3DCOLOR_XRGB(255,255,255));
					  }
					  */
					 }
#endif

