#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHContactBodyEffector.h"

#include "ParticlesObject.h"
 

///////////////////////////////////////////////////////////////
#pragma warning(disable:4995)
#pragma warning(disable:4267)
#include "../ode/src/collision_kernel.h"
#include <../ode/src/joint.h>
#include <../ode/src/objects.h>
#pragma warning(default:4267)
#pragma warning(default:4995)

#include "gameobject.h"
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"
//union dInfBytes dInfinityValue = {{0,0,0x80,0x7f}};
PhysicsStepTimeCallback		*physics_step_time_callback=0;
const dReal fixed_step=0.02f;
const u32	dis_frames=11;
const dReal default_w_limit = M_PI/16.f/fixed_step;
const dReal default_l_limit = 3.f/fixed_step;
const dReal default_l_scale	=1.01f;
const dReal default_w_scale	=1.01f;
const dReal	default_k_l		=0.0002f;//square resistance !!
const dReal	default_k_w		=0.05f;


#ifndef  ODE_SLOW_SOLVER
const dReal world_cfm		=1.1363636e-008f;
const dReal world_erp		=0.74545456f;
#else
const dReal world_cfm		=1.1363636e-006f;
const dReal world_erp		=0.54545456f;
#endif

const dReal world_spring	=1.0f*SPRING	(world_cfm,world_erp);
const dReal world_damping	=1.0f*DAMPING	(world_cfm,world_erp);


const dReal	world_gravity	= 2.f*9.81f;

int			phFPS			= 50;
int			phIterations	= 18;
float		phTimefactor	= 1.f;
int			phIterationCycle= 5;

static int IterationCycleI= 0;
dWorldID	phWorld;
/////////////////////////////////////
dJointGroupID ContactGroup;
CBlockAllocator<dJointFeedback,128>			ContactFeedBacks;
CBlockAllocator<CPHContactBodyEffector,128> ContactEffectors;
#ifdef DRAW_CONTACTS
CONTACT_VECTOR Contacts;
#endif
///////////////////////////////////////////////////////////
class SApplyBodyEffectorPred
{

public:
	SApplyBodyEffectorPred()
	{
	}

	void	operator	()(CPHContactBodyEffector* pointer) const
	{
		pointer->Apply();
	}
};
/////////////////////////////////////////////////////////////////////////////
IC void add_contact_body_effector(dBodyID body,dContact& c,float flotation)
{
	CPHContactBodyEffector* effector=(CPHContactBodyEffector*)dBodyGetData(body);

	if(effector)
		effector->Merge(c,flotation);
	else
	{	
		effector=ContactEffectors.add();
		effector->Init(body,c,flotation);
		dBodySetData(body,(void*)effector);
	}
}








IC static bool CollideIntoGroup(dGeomID o1, dGeomID o2,dJointGroupID jointGroup)
{
	const ULONG N = 800;
	static dContact contacts[N];
	bool	collided=false;
	// get the contacts up to a maximum of N contacts
	ULONG n;
	
	n = dCollide(o1, o2, N, &contacts[0].geom, sizeof(dContact));	

	if(n>N-1)
		n=N-1;
	ULONG i;
	

	for(i = 0; i < n; ++i)
	{
		dContact				&c		=contacts[i];
		dContactGeom			&cgeom	=c.geom;
		dSurfaceParameters		&surface=c.surface;
		dGeomID					g1		=cgeom.g1;
		dGeomID					g2		=cgeom.g2;
		if(!i) {
			dReal dif=	dFabs(contacts[i-1].geom.pos[0]-cgeom.pos[0])+
				dFabs(contacts[i-1].geom.pos[1]-cgeom.pos[1])+
				dFabs(contacts[i-1].geom.pos[2]-cgeom.pos[2]);
			if(fis_zero (dif)) continue;
		}

		bool pushing_neg=	false;
		bool do_collide	=	true;
		dxGeomUserData* usr_data_1		=NULL;
		dxGeomUserData* usr_data_2		=NULL;
		u16				material_id_1	=0;
		u16				material_id_2	=0;

		surface.mu =1.f;// 5000.f;
		surface.soft_erp=1.f;//ERP(world_spring,world_damping);
		surface.soft_cfm=1.f;//CFM(world_spring,world_damping);
		surface.bounce = 0.01f;//0.1f;
		surface.bounce_vel =1.5f;//0.005f;
		usr_data_1 = retrieveGeomUserData(g1);
		usr_data_2 = retrieveGeomUserData(g2);
		///////////////////////////////////////////////////////////////////////////////////////////////////
		if(usr_data_2)	material_id_2=usr_data_2->material;
		if(usr_data_1)	material_id_1=usr_data_1->material;
		bool is_tri_1=dTriListClass == dGeomGetClass(g1);
		bool is_tri_2=dTriListClass == dGeomGetClass(g2);
		if(!is_tri_2&&!is_tri_1) surface.mode=0;
		if(is_tri_1) material_id_1=(u16)surface.mode;
		if(is_tri_2) material_id_2=(u16)surface.mode;
		SGameMtl* material_1=GMLib.GetMaterialByIdx(material_id_1);
		SGameMtl* material_2=GMLib.GetMaterialByIdx(material_id_2);
		////////////////params can be changed in callbacks//////////////////////////////////////////////////////////////////////////
		float spring	=material_2->fPHSpring*material_1->fPHSpring*world_spring;
		float damping	=material_2->fPHDamping*material_1->fPHDamping*world_damping;
		surface.soft_erp=ERP(spring,damping);
		surface.soft_cfm=CFM(spring,damping);
		surface.mu=material_2->fPHFriction*material_1->fPHFriction;
		/////////////////////////////////////////////////////////////////////////////////////////////////
		if(usr_data_2&&usr_data_2->object_callback){
			usr_data_2->object_callback(do_collide,c);
		}

		if(usr_data_1&&usr_data_1->object_callback){
			usr_data_1->object_callback(do_collide,c);
		}

		if(usr_data_2){
			usr_data_2->pushing_b_neg	=	usr_data_2->pushing_b_neg	&& !GMLib.GetMaterialByIdx(usr_data_2->b_neg_tri.T->material)->Flags.is(SGameMtl::flPassable);
			usr_data_2->pushing_neg		=	usr_data_2->pushing_neg		&& !GMLib.GetMaterialByIdx(usr_data_2->neg_tri.T->material)->Flags.is(SGameMtl::flPassable);
			pushing_neg=usr_data_2->pushing_b_neg||usr_data_2->pushing_neg;
			if(usr_data_2->ph_object){
				usr_data_2->ph_object->InitContact(&c,do_collide);
			}

		}
		///////////////////////////////////////////////////////////////////////////////////////
		if(usr_data_1){ 
			usr_data_1->pushing_b_neg	=	usr_data_1->pushing_b_neg	&& !GMLib.GetMaterialByIdx(usr_data_1->b_neg_tri.T->material)->Flags.is(SGameMtl::flPassable);
			usr_data_1->pushing_neg		=	usr_data_1->pushing_neg		&& !GMLib.GetMaterialByIdx(usr_data_1->neg_tri.T->material)->Flags.is(SGameMtl::flPassable);
			pushing_neg=usr_data_1->pushing_b_neg||usr_data_1->pushing_neg;
			if(usr_data_1->ph_object){
				usr_data_1->ph_object->InitContact(&c,do_collide);

			}
		}

		Flags32	&flags_1=material_1->Flags;
		Flags32	&flags_2=material_2->Flags;

		if(is_tri_1)
		{
#pragma warning(push)
#pragma warning(disable:4245)
			if(material_1->Flags.is(SGameMtl::flSlowDown)&&!(usr_data_2->pushing_neg||usr_data_2->pushing_b_neg))
#pragma warning(pop)
			{
				dBodyID body=dGeomGetBody(g2);
				R_ASSERT2(body,"static - static collision !!!");
				add_contact_body_effector(body,c,material_1->fFlotationFactor);
			}
			if(material_1->Flags.is(SGameMtl::flPassable)) 
				do_collide=false;
		}
		if(is_tri_2)
		{
#pragma warning(push)
#pragma warning(disable:4245)
			if(material_2->Flags.is(SGameMtl::flSlowDown)&&!(usr_data_1->pushing_neg||usr_data_1->pushing_b_neg))
#pragma warning(pop)
			{

				dBodyID body=dGeomGetBody(g1);
				R_ASSERT2(body,"static - static collision !!!");
				add_contact_body_effector(body,c,material_2->fFlotationFactor);
			}
			if(material_2->Flags.is(SGameMtl::flPassable)) 
				do_collide=false;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////params can not be changed by calbacks/////////////////////////////////
		surface.mode =dContactApprox1|dContactSoftERP|dContactSoftCFM;
		if(flags_1.is(SGameMtl::flBounceable)&&flags_2.is(SGameMtl::flBounceable))
		{
			surface.mode		|=	dContactBounce;
			surface.bounce_vel	=	_max(material_1->fPHBounceStartVelocity,material_2->fPHBounceStartVelocity);
			surface.bounce		=	_min(material_1->fPHBouncing,material_2->fPHBouncing);
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////
		if	(pushing_neg)
			surface.mu=dInfinity;
		if	(do_collide)
		{
			collided=true;
			#ifdef DRAW_CONTACTS
			Contacts.push_back(c);
			#endif
			dJointID contact_joint	= dJointCreateContact(phWorld, jointGroup, &c);
			dJointAttach			(contact_joint, dGeomGetBody(g1), dGeomGetBody(g2));
		}
	}
	return collided;
}
void NearCallback(CPHObject* obj1,CPHObject* obj2, dGeomID o1, dGeomID o2)
{	
	if(CollideIntoGroup(o1,o2,ContactGroup) && obj2 &&!obj2->is_active())obj2->EnableObject();
}
void CollideStatic(dGeomID o2)
{
	CollideIntoGroup(ph_world->GetMeshGeom(),o2,ContactGroup);
}

void SaveContactsStatic(dGeomID o2,dJointGroupID jointGroup)
{
	CollideIntoGroup(ph_world->GetMeshGeom(),o2,jointGroup);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void SaveContacts(CPHObject* obj1,CPHObject* obj2,dGeomID o1, dGeomID o2,dJointGroupID jointGroup)
{
	CollideIntoGroup(o1,o2,jointGroup);
}

void __stdcall ContactShotMark(CDB::TRI* T,dContactGeom* c)
{
	dBodyID b=dGeomGetBody(c->g1);
	dxGeomUserData* data;
	if(!b) 
	{
		b=dGeomGetBody(c->g2);
		data=dGeomGetUserData(c->g2);
	}
	else
	{
		data=dGeomGetUserData(c->g1);
	}
	if(!b) return;
	dVector3 vel;
	dMass m;
	dBodyGetMass(b,&m);

	dBodyGetPointVel(b,c->pos[0],c->pos[1],c->pos[2],vel);
	dReal vel_cret=dFabs(dDOT(vel,c->normal))* _sqrt(m.mass);
	{
		if(data)
		{
			SGameMtlPair* mtl_pair		= GMLib.GetMaterialPair(T->material,data->material);
			//	char buf[40];
			//	R_ASSERT3(mtl_pair,strconcat(buf,"Undefined material pair:  # ", GMLib.GetMaterial(T->material)->name),GMLib.GetMaterial(data->material)->name);
			if(mtl_pair)
			{
				if(vel_cret>30.f && !mtl_pair->CollideMarks.empty())
				{

					ref_shader pWallmarkShader = mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];;

					//добавить отметку на материале
					::Render->add_Wallmark(pWallmarkShader, *((Fvector*)c->pos), 
						0.09f, T,
						Level().ObjectSpace.GetStaticVerts());

					//::Render->add_Wallmark	(
					//SELECT_RANDOM(mtl_pair->HitMarks),
					//*((Fvector*)c->pos),
					//0.09f,
					//T);

				}

				SGameMtl* mtl = NULL;
				SGameMtl* mtl1 = NULL;
				if(vel_cret>10.0f)
				{
					mtl  = GMLib.GetMaterialByID(mtl_pair->GetMtl0());
					mtl1 = GMLib.GetMaterialByID(mtl_pair->GetMtl1());

					int a = 0;
					a++;
				}

				if(vel_cret>10.0f && !mtl_pair->CollideSounds.empty())
				{
					::Sound->play_at_pos(
						SELECT_RANDOM(mtl_pair->CollideSounds) ,0,*((Fvector*)c->pos)
						);
				}



				if(vel_cret>15.f && !mtl_pair->CollideParticles.empty())
				{
					LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];
					//отыграть партиклы столкновения материалов
					CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

					Fmatrix pos; 
					Fvector zero_vel = {0.f,0.f,0.f};
					pos.k.set(*((Fvector*)c->normal));
					Fvector::generate_orthonormal_basis(pos.k, pos.i, pos.j);
					pos.c.set(*((Fvector*)c->pos));

					ps->UpdateParent(pos,zero_vel);
					Level().ps_needtoplay.push_back(ps);
				}
			}
		}

		//			::Render->add_Wallmark	(
		//				CPHElement::hWallmark,
		//				*((Fvector*)c->pos),
		//				0.09f,
		//				T);

	} 
}


//half square time

/*
void BodyCutForce(dBodyID body)
{
dReal linear_limit=l_limit;
//applyed force
const dReal* force=	dBodyGetForce(body);

//body mass
dMass m;
dBodyGetMass(body,&m);

//accceleration correspondent to the force
dVector3 a={force[0]/m.mass,force[1]/m.mass,force[2]/m.mass};

//current velocity
const dReal* start_vel=dBodyGetLinearVel(body);

//velocity adding during one step
dVector3 add_vel={a[0]*fixed_step,a[1]*fixed_step,a[2]*fixed_step};

//result velocity
dVector3 vel={start_vel[0]+add_vel[0],start_vel[1]+add_vel[1],start_vel[2]+add_vel[2]};

//result velocity magnitude
dReal speed=dSqrt(dDOT(vel,vel));

if(speed>linear_limit) //then we need to cut applied force
{
//solve the triangle - cutted velocity - current veocity - add velocity 
//to find cutted adding velocity

//add_vell magnitude
dReal add_speed=dSqrt(dDOT(add_vel,add_vel));

//current velocity magnitude
dReal start_speed=dSqrt(dDOT(start_vel,start_vel));

//cosinus of the angle between vel ang add_vell
dReal cosinus1=dFabs(dDOT(add_vel,start_vel)/start_speed/add_speed);

dReal cosinus1_2=cosinus1*cosinus1;
if(cosinus1_2>1.f)
cosinus1_2=1.f;
dReal cutted_add_speed;
if(cosinus1_2==1.f)
cutted_add_speed=linear_limit-start_speed;
else
{
//sinus 
dReal sinus1_2=1.f-cosinus1_2;


dReal sinus1=dSqrt(sinus1_2);


//sinus of the angle between cutted velocity and adding velociity (sinus theorem)
dReal sinus2=sinus1/linear_limit*start_speed;
dReal sinus2_2=sinus2*sinus2;
if(sinus2_2>1.f)sinus2_2=1.f;

dReal cosinus2_2=1.f-sinus2_2;
dReal cosinus2=dSqrt(cosinus2_2);

//sinus of 180-ang1-ang2
dReal sinus3=sinus1*cosinus2+cosinus1*sinus2;

//cutted adding velocity magnitude (sinus theorem)

cutted_add_speed=linear_limit/sinus1*sinus3;
}
//substitude force

dBodyAddForce(body,
cutted_add_speed/add_speed/fixed_step*m.mass*add_vel[0]-force[0],
cutted_add_speed/add_speed/fixed_step*m.mass*add_vel[1]-force[1],
cutted_add_speed/add_speed/fixed_step*m.mass*add_vel[2]-force[2]	  
);
}

}
*/
//limit for angular accel
void dBodyAngAccelFromTorqu(const dBodyID body, dReal* ang_accel, const dReal* torque){
	dMass m;
	dMatrix3 invI;
	dBodyGetMass(body,&m);
	dInvertPDMatrix (m.I, invI, 3);
	dMULTIPLY1_333(ang_accel,invI, torque);
}

void FixBody(dBodyID body)
{
	FixBody(body,10000000.f,10000000000000.f);
}
void FixBody(dBodyID body,float ext_param,float mass_param)
{
	dMass m;
	dMassSetSphere(&m,1.f,ext_param);
	dMassAdjust(&m,mass_param);
	dBodySetMass(body,&m);
	dBodySetGravityMode(body,0);
}

void BodyCutForce(dBodyID body,float l_limit,float w_limit)
{
	const dReal wa_limit=w_limit/fixed_step;
	const dReal* force=	dBodyGetForce(body);
	dReal force_mag=dSqrt(dDOT(force,force));

	//body mass
	dMass m;
	dBodyGetMass(body,&m);

	dReal force_limit =l_limit/fixed_step*m.mass;

	if(force_mag>force_limit)
	{
		dBodySetForce(body,
			force[0]/force_mag*force_limit,
			force[1]/force_mag*force_limit,
			force[2]/force_mag*force_limit
			);
	}


	const dReal* torque=dBodyGetTorque(body);
	dReal torque_mag=dSqrt(dDOT(torque,torque));

	if(torque_mag<0.001f) return;

	dMatrix3 tmp,invI,I;

	// compute inertia tensor in global frame
	dMULTIPLY2_333 (tmp,m.I,body->R);
	dMULTIPLY0_333 (I,body->R,tmp);

	// compute inverse inertia tensor in global frame
	dMULTIPLY2_333 (tmp,body->invI,body->R);
	dMULTIPLY0_333 (invI,body->R,tmp);

	//angular accel
	dVector3 wa;
	dMULTIPLY0_331(wa,invI,torque);
	dReal wa_mag=dSqrt(dDOT(wa,wa));

	if(wa_mag>wa_limit)
	{
		//scale w 
		for(int i=0;i<3;++i)wa[i]*=wa_limit/wa_mag;
		dVector3 new_torqu;

		dMULTIPLY0_331(new_torqu,I,wa);

		dBodySetTorque 
			(
			body,
			new_torqu[0],
			new_torqu[1],
			new_torqu[2]
			);
	}
}

void dMassSub(dMass *a,const dMass *b)
{
	int i;
	dAASSERT (a && b);
	dReal denom = dRecip (a->mass-b->mass);
	for (i=0; i<3; ++i) a->c[i] = (a->c[i]*a->mass - b->c[i]*b->mass)*denom;
	a->mass-=b->mass;
	for (i=0; i<12; ++i) a->I[i] -= b->I[i];
}

void __stdcall PushOutCallback(bool& do_colide,dContact& c)
{

	dBodyID body1=dGeomGetBody(c.geom.g1);
	dBodyID body2=dGeomGetBody(c.geom.g2);
	if(!(body1&&body2)) 
		return;

	const dReal* pos1=dGeomGetPosition(c.geom.g1);
	const dReal* pos2=dGeomGetPosition(c.geom.g2);


	//do_colide=false;
	//return;
	if(c.geom.depth>0.005f)
	{
		/*
		dxGeomUserData* usr_data_1=NULL;
		dxGeomUserData* usr_data_2=NULL;
		usr_data_1 = retrieveGeomUserData(c.geom.g1);
		usr_data_2 = retrieveGeomUserData(c.geom.g2);

		c.surface.soft_erp=2500.f;
		c.surface.soft_cfm=0.000002f;
		if(usr_data_1&&usr_data_2)
		if(usr_data_1->object_callback==PushOutCallback&&usr_data_1->object_callback==PushOutCallback)
		*/
		const dReal erp=0.2f;
		const dReal cfm=0.001f;



		//c.surface.soft_erp=(1.f-erp)/cfm /world_damping;
		//c.surface.soft_cfm=erp/cfm/fixed_step /world_spring;
		c.surface.soft_erp=erp;
		c.surface.soft_cfm=cfm;
	}

	if(pos1[0]==pos2[0]&&pos1[1]==pos2[1]&&pos1[2]==pos2[2])

	{
		do_colide=false;
		const dReal force=1.f;

		{	dMass m;
		dBodyGetMass(body1,&m);
		dBodyAddForce(body1,-c.geom.normal[0]*m.mass*force,-c.geom.normal[1]*m.mass*force,-c.geom.normal[2]*m.mass*force);


		dBodyGetMass(body2,&m);
		dBodyAddForce(body2,c.geom.normal[0]*m.mass*force,c.geom.normal[1]*m.mass*force,c.geom.normal[2]*m.mass*force);
		do_colide=false;
		}


	}


	//if()
}



void __stdcall PushOutCallback1(bool& do_colide,dContact& c)
{

	dBodyID body1=dGeomGetBody(c.geom.g1);
	dBodyID body2=dGeomGetBody(c.geom.g2);
	if(!(body1&&body2)) 
		return;

	///const dReal* pos1=dGeomGetPosition(c.geom.g1);
	//const dReal* pos2=dGeomGetPosition(c.geom.g2);


	dxGeomUserData* usr_data_1=NULL;
	dxGeomUserData* usr_data_2=NULL;
	usr_data_1 = retrieveGeomUserData(c.geom.g1);
	usr_data_2 = retrieveGeomUserData(c.geom.g2);

	CGameObject* obj1=dynamic_cast<CGameObject*>(usr_data_1->ph_ref_object);
	CGameObject* obj2=dynamic_cast<CGameObject*>(usr_data_2->ph_ref_object);

	if(obj1 && obj2 && obj1->m_pPhysicsShell && obj2->m_pPhysicsShell )
	{	

		do_colide=false;
		Fvector push_line,force;
		push_line.sub(obj1->Position(),obj2->Position());
		float mag =push_line.magnitude();


		dMass m1,m2;
		dBodyGetMass(body1,&m1);
		dBodyGetMass(body2,&m2);

		if(m1.mass<m2.mass)
		{
			force.mul(push_line,m1.mass/mag*50.f);
		}
		else
		{
			force.mul(push_line,m2.mass/mag*50.f);
		}

		dBodyAddForce(body1,force.x,force.y,force.z);
		dBodyAddForce(body2,-force.x,-force.y,-force.z);
	}
}
void __stdcall PushOutCallback2(bool& do_colide,dContact& c)
{

	dBodyID body1=dGeomGetBody(c.geom.g1);
	dBodyID body2=dGeomGetBody(c.geom.g2);
	if(!(body1&&body2)) 
		return;
	//c.surface.soft_erp*=3.1623f;
	MulSprDmp(c.surface.soft_cfm,c.surface.soft_erp,1.f,3.1623f);
}
//
float E_NlS(dBodyID body,dReal* norm,float norm_sign)
{
	const dReal* vel=dBodyGetLinearVel(body);
	dReal prg=-dDOT(vel,norm)*norm_sign;
	prg=prg<0.f ? prg=0.f : prg;
	dMass mass;
	dBodyGetMass(body,&mass);
	return mass.mass*prg*prg/2;
}

void ApplyGravityAccel(dBodyID body,const dReal* accel)
{
	dMass m;
	dBodyGetMass(body,&m);
	dBodyAddForce(body,accel[0]*m.mass,accel[1]*m.mass,accel[2]*m.mass);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
