#include "stdafx.h"
#include "climableobject.h "

	CClimableObject::CClimableObject		()
{

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
	return inherited::net_Spawn(DC);
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