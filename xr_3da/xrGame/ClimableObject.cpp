#include "stdafx.h"
#include "climableobject.h "
#include "PHStaticGeomShell.h"
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
	m_pStaticShell=P_BuildStaticGeomShell(smart_cast<CGameObject*>(this),0);
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