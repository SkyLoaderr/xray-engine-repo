/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void __stdcall BoneCallbackFun(CBoneInstance* B);
typedef  void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	 void __stdcall ObjectContactCallbackFun(bool& do_colide,dContact& c);
struct Fcylinder;
#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
///////////////////////////////////////////////////////////////
///#pragma warning(disable:4995)
//#include "..\ode\src\collision_kernel.h"
//#include <..\ode\src\joint.h>
//#include <..\ode\src\objects.h>

//#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHShell.h"

void CPHShell::setDensity(float M){
	xr_vector<CPHElement*>::iterator i;
	//float volume=0.f;
	//for(i=elements.begin();i!=elements.end();i++)	volume+=(*i)->get_volume();

	for(i=elements.begin();i!=elements.end();i++)
		(*i)->setDensity(M);
}


void CPHShell::setMass(float M){
	xr_vector<CPHElement*>::iterator i;
	float volume=0.f;
	for(i=elements.begin();i!=elements.end();i++)	volume+=(*i)->get_volume();

	for(i=elements.begin();i!=elements.end();i++)
		(*i)->setMass(
		(*i)->get_volume()/volume*M
		);
}

void CPHShell::setMass1(float M){
	xr_vector<CPHElement*>::iterator i;


	for(i=elements.begin();i!=elements.end();i++)
		(*i)->setMass(
		M/elements.size()
		);
}
float CPHShell::getMass()
{
	float m=0.f;

	xr_vector<CPHElement*>::iterator i;

	for(i=elements.begin();i!=elements.end();i++)	m+=(*i)->getMass();

	return m;
}
void CPHShell::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){
	if(bActive)
		return;
	m_ident=ph_world->AddObject(this);

	xr_vector<CPHElement*>::iterator i;

	mXFORM.set(m0);
	m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	//dSpaceSetCleanup (m_space, 0);
	for(i=elements.begin();i!=elements.end();i++){
		//(*i)->Start();
		//(*i)->SetTransform(m0);
		(*i)->Activate(m0,dt01, m2, disable);
	}
	//SetPhObjectInElements();
	bActive=true;
}



void CPHShell::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	if(bActive)
		return;
	m_ident=ph_world->AddObject(this);

	xr_vector<CPHElement*>::iterator i;

	mXFORM.set(transform);
	m_space=dSimpleSpaceCreate(ph_world->GetSpace());
	//dSpaceSetCleanup (m_space, 0);
	for(i=elements.begin();i!=elements.end();i++){
		//(*i)->Start();
		//(*i)->SetTransform(m0);
		(*i)->Activate(transform,lin_vel, ang_vel);
	}
	//SetPhObjectInElements();
	bActive=true;
}


void CPHShell::Deactivate(){
	if(!bActive)
		return;
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->Deactivate();

	xr_vector<CPHJoint*>::iterator j;
	for(j=joints.begin();j!=joints.end();j++)
		(*j)->Deactivate();

	ph_world->RemoveObject(m_ident);
	if(m_space) {
		dSpaceDestroy(m_space);
		m_space=NULL;
	}
	bActive=false;
	bActivating=false;
}

void CPHShell::PhDataUpdate(dReal step){

	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->PhDataUpdate(step);
}



void CPHShell::PhTune(dReal step){
}

void CPHShell::Update(){
	if(!bActive) return;
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->Update();
	mXFORM.set((*elements.begin())->mXFORM);
}


void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val){
	if(!bActive) return;
	(*elements.begin())->applyImpulseTrace		( pos,  dir,  val);
}

void	CPHShell::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const s16 element){
	if(!bActive) return;
	VERIFY(p_kinematics);
	CBoneInstance& instance=p_kinematics->LL_GetInstance				(element);
	if(!instance.Callback_Param) return;
	((CPHElement*)instance.Callback_Param)->applyImpulseTrace		( pos,  dir,  val);

}


void __stdcall CPHShell:: BonesCallback				(CBoneInstance* B){
	CPHElement*	E			= dynamic_cast<CPHElement*>	(static_cast<CObject*>(B->Callback_Param));

	E->CallBack(B);
}



void CPHShell::Activate(bool place_current_forms,bool disable){
	if(bActive)
		return;

		m_ident=ph_world->AddObject(this);
		{
		xr_vector<CPHElement*>::iterator i;
		if(place_current_forms)
		for(i=elements.begin();i!=elements.end();i++)	{
														//(*i)->Start();
														//(*i)->SetTransform(m0);
														(*i)->Activate(mXFORM,disable);
														}
		}

		{
		
		xr_vector<CPHJoint*>::iterator i;
		for(i=joints.begin();i!=joints.end();i++) (*i)->Activate();
		}
	//SetPhObjectInElements();/////////////////////////////////////////////////////////////////////
	bActive=true;
	bActivating=true;
}

void CPHShell::SetTransform(Fmatrix m){
	Fmatrix init;
	xr_vector<CPHElement*>::iterator i=elements.begin();
	(*i)->InterpolateGlobalTransform(&init);
	init.invert();
	Fmatrix add;
	add.mul(init,m);
	(*i)->SetTransform(m);
	i++;
	Fmatrix element_transform;
	for( ;i!=elements.end(); i++)
	{
		(*i)->InterpolateGlobalTransform(&element_transform);
		element_transform.mulA(add);
		(*i)->SetTransform(element_transform);
	}


}


void CPHShell::Enable()
{
	if(!bActive)
		return;
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->Enable();
}

void CPHShell::set_PhysicsRefObject	 (CPhysicsRefObject* ref_object)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->set_PhysicsRefObject(ref_object);
	}
}



void CPHShell::set_ContactCallback(ContactCallbackFun* callback)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->set_ContactCallback(callback);
}


void CPHShell::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
		(*i)->set_ObjectContactCallback(callback);
}

void CPHShell::SetPhObjectInElements()
{
	if(!bActive) return;
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++ )
		(*i)->SetPhObjectInGeomData((CPHObject*)this);
}

void CPHShell::SetMaterial(LPCSTR m)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->SetMaterial(m);
	}
}

void CPHShell::SetMaterial(u32 m)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->SetMaterial(m);
	}
}

void CPHShell::get_LinearVel(Fvector& velocity)
{

	(*elements.begin())->get_LinearVel(velocity);
}

void CPHShell::set_PushOut(u32 time,PushOutCallbackFun* push_out)
{
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{
		(*i)->set_PushOut(time,push_out);
	}
}


void CPHShell::SmoothElementsInertia(float k)
{
	dMass m_avrg;
	dReal krc=1.f-k;
	dMassSetZero(&m_avrg);
	xr_vector<CPHElement*>::iterator i;
	for(i=elements.begin();i!=elements.end();i++)
	{

		dMassAdd(&m_avrg,(*i)->GetMass());

	}
	int n = (int)elements.size();
	m_avrg.mass/=n*k;
	for(int j=0;j<4*3;j++) m_avrg.I[j]/=n*k;

	for(i=elements.begin();i!=elements.end();i++)
	{
		dVector3 tmp;
		dMass* m=(*i)->GetMass();
		Memory.mem_copy(tmp,m->c,sizeof(dVector3));

		m->mass*=krc;
		for(int j=0;j<4*3;j++) m->I[j]*=krc;
		dMassAdd(m,&m_avrg);

		Memory.mem_copy(m->c,tmp,sizeof(dVector3));
	}
}