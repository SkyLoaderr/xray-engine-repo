#include "stdafx.h"
#include "car.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCar::SExhaust::~SExhaust()
{
	if(p_pgobject)xr_delete(p_pgobject);
}

void CCar::SExhaust::Init()
{
	pelement=(bone_map.find(bone_id))->second.element;
	CKinematics* K=smart_cast<CKinematics*>(pcar->Visual());
	CBoneData&	bone_data=K->LL_GetData(u16(bone_id));
	transform.set(bone_data.bind_transform);
	///transform.mulA(pcar->XFORM());
	//Fmatrix element_transform;
	//pelement->InterpolateGlobalTransform(&element_transform);
	//element_transform.invert();
	//transform.mulA(element_transform);
	p_pgobject=xr_new<CParticlesObject>(*pcar->m_exhaust_particles,pcar->Sector(),false);
	Fvector zero_vector;
	zero_vector.set(0.f,0.f,0.f);
	p_pgobject->UpdateParent(pcar->XFORM(), zero_vector );

}

void CCar::SExhaust::Update()
{
	Fmatrix global_transform;
	pelement->InterpolateGlobalTransform(&global_transform);
	global_transform.mulB(transform);
	dVector3 res;
	Fvector	 res_vel;
	dBodyGetPointVel(pelement->get_body(),global_transform.c.x,global_transform.c.y,global_transform.c.z,res);
	Memory.mem_copy (&res_vel,res,sizeof(Fvector));
	//velocity.mul(0.95f);
	//res_vel.mul(0.05f);
	//velocity.add(res_vel);
	p_pgobject->UpdateParent(global_transform,res_vel);
}

void CCar::SExhaust::Clear()
{
	xr_delete(p_pgobject);
}

void CCar::SExhaust::Play()
{
	p_pgobject->Play();
	Update();
}

void CCar::SExhaust::Stop()
{
	p_pgobject->Stop();
}