#include "stdafx.h"
#include "climableobject.h "
#include "PHStaticGeomShell.h"
#include "xrServer_Objects_ALife.h"
	CClimableObject::CClimableObject		()
{
	m_pStaticShell=NULL;
}
	CClimableObject::~CClimableObject	()
{

}
void CClimableObject::	Load				( LPCSTR section)
{
	inherited::Load(section);
}
BOOL CClimableObject::	net_Spawn			( LPVOID DC)
{
	BOOL ret	= inherited::net_Spawn(DC);
	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeObjectClimable	*CLB=smart_cast<CSE_ALifeObjectClimable*>(e);
	Fmatrix& b=CLB->shapes[0].data.box;
	m_box.xform_set(XFORM());
	m_box.m_halfsize.set(b._11,b._22,b._33);
	m_radius=_max(_max(m_box.m_halfsize.x,m_box.m_halfsize.y),m_box.m_halfsize.z);
	m_pStaticShell=P_BuildStaticGeomShell(smart_cast<CGameObject*>(this),0,m_box);
	return ret;
}
void CClimableObject::	net_Destroy			()
{
	inherited::net_Destroy();
}
void CClimableObject::	shedule_Update		( u32 dt)							// Called by shedule
{
	inherited::shedule_Update(dt);
}
void CClimableObject::	UpdateCL			( )								// Called each frame, so no need for d
{
	inherited::UpdateCL();
}

void	CClimableObject::Center				(Fvector &C) const
{
	C.set(XFORM().c);
}
float	CClimableObject::Radius				() const
{
		return							m_radius;
}