#include "stdafx.h"
#pragma hdrstop

#include "BreakableObject.h"
#include "xrserver_objects_alife.h"
#include "PHStaticGeomShell.h"
#include "PhysicsShell.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
u32 CBreakableObject::m_remove_time=0;
CBreakableObject::CBreakableObject	()
{
	Init();
}

CBreakableObject::~CBreakableObject	()
{
}

void CBreakableObject::Load		(LPCSTR section)
{
	inherited::Load			(section);
	m_remove_time=pSettings	->r_u32(section,"remove_time")*1000;
}

BOOL CBreakableObject::net_Spawn(LPVOID DC)
{
	CSE_Abstract			*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectBreakable *obj	= dynamic_cast<CSE_ALifeObjectBreakable*>(e);
	R_ASSERT				(obj);
	inherited::net_Spawn	(DC);
	collidable.model = xr_new<CCF_Skeleton>(this);
	// set bone id
	R_ASSERT				(Visual()&&PKinematics(Visual()));
//	CKinematics* K			= PKinematics(Visual());
	fHealth					= obj->m_health;
	
	setVisible				(TRUE);
	setEnabled				(TRUE);
	CreateUnbroken			();
	CreateBroken			();
	bRemoved				=false;
	//Break					();
	return					(TRUE);
}

void CBreakableObject::shedule_Update	(u32 dt)
{
	inherited::shedule_Update		(dt);
	if(m_pPhysicsShell&&!bRemoved&&Device.dwTimeGlobal-m_break_time>m_remove_time) SendDestroy();
}

void CBreakableObject::UpdateCL	()
{
	inherited::UpdateCL		();
	if(m_pPhysicsShell)m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
}

void CBreakableObject::renderable_Render()
{
	inherited::renderable_Render	();
}

void CBreakableObject::Hit(float P,Fvector &dir, CObject* who,s16 element,
					   Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/)
{
	
	if(m_pPhysicsShell)m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
	Break					();

}

void CBreakableObject::net_Export(NET_Packet& P)
{
	VERIFY					(Local());
}

void CBreakableObject::net_Import(NET_Packet& P)
{
	VERIFY					(Remote());
}

BOOL CBreakableObject::UsedAI_Locations()
{
	return					(FALSE);
}

void CBreakableObject::CreateUnbroken()
{
	m_pUnbrokenObject=xr_new<CPHStaticGeomShell>();
	Fobb b;
	Visual()->vis.box.getradius(b.m_halfsize);
	b.xform_set(Fidentity);
	m_pUnbrokenObject->add_Box(b);
	m_pUnbrokenObject->Activate(XFORM());
	m_pUnbrokenObject->set_PhysicsRefObject(this);
	//m_pUnbrokenObject->SetPhObjectInGeomData(m_pUnbrokenObject);
	m_pUnbrokenObject->set_ObjectContactCallback(ObjectContactCallback);
}
void CBreakableObject::DestroyUnbroken()
{
	if(!m_pUnbrokenObject) return;
	m_pUnbrokenObject->Deativate();
	xr_delete(m_pUnbrokenObject);
}

void CBreakableObject::CreateBroken()
{
	m_Shell=P_create_Shell();
	m_Shell->preBuild_FromKinematics(PKinematics(Visual()));
	m_Shell->mXFORM.set(XFORM());
	//m_Shell->SetAirResistance(0.002f*skel_airr_lin_factor,
	//	0.3f*skel_airr_ang_factor);
	m_Shell->set_PhysicsRefObject(this);
	m_Shell->Build();
	m_Shell->setDensity(1000.f);
	dMass m;
	dMassSetBox(&m,m_Shell->getMass()/100.f,1.f,1.f,1.f);
	m_Shell->addEquelInertiaToEls(m);
	m_Shell->SmoothElementsInertia(0.3f);
}

void CBreakableObject::ActivateBroken()
{
	m_pPhysicsShell=m_Shell;
	m_pPhysicsShell->RunSimulation();
	m_pPhysicsShell->SetCallbacks();
	PKinematics(Visual())->Calculate();
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());
}

void CBreakableObject::net_Destroy()
{
	DestroyUnbroken();
	if(m_Shell)
	{
		m_Shell->Deactivate();
		xr_delete(m_Shell);
	}
	m_pPhysicsShell=NULL;
	inherited::net_Destroy();
	xr_delete(collidable.model);
}
void CBreakableObject::Split()
{
}

void CBreakableObject::Break()
{
	if(m_pPhysicsShell)return;
	DestroyUnbroken();
	ActivateBroken();
	u16 el_num=m_pPhysicsShell->get_ElementsNumber();
	for(u16 i=0;i<el_num;i++)
	{
		Fvector pos,dir;
		pos.set(Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f));
		dir.set(Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f),Random.randF(-0.3f,0.3f));
		dir.normalize();
		m_pPhysicsShell->get_ElementByStoreOrder(i)->applyImpulseTrace(pos,dir,Random.randF(1.f,10.f),0);
	}
	m_break_time=Device.dwTimeGlobal;
}

void CBreakableObject::SendDestroy()
{
	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,ID());
	Msg					("ge_destroy: [%d] - %s",ID(),*cName());
	if (Local()) u_EventSend			(P);
	bRemoved=true;
}

void CBreakableObject::ObjectContactCallback(bool&/**do_colide/**/,dContact& c)
{
	
}

void CBreakableObject::Init()
{
	fHealth					= 100.f;
	m_pUnbrokenObject		= NULL;
	m_Shell					= NULL;
	bRemoved				= false;
}