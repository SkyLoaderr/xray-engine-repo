///////////////////////////////////////////////////////////////
// Antirad.cpp
// Antirad - таблетки выводящие радиацию
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "antirad.h"
#include "PhysicsShell.h"

CAntirad::CAntirad(void) 
{
}

CAntirad::~CAntirad(void) 
{
}


BOOL CAntirad::net_Spawn(LPVOID DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CAntirad::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CAntirad::net_Destroy() 
{
	inherited::net_Destroy();
}

void CAntirad::shedule_Update(u32 dt) 
{
	inherited::shedule_Update(dt);
}

void CAntirad::UpdateCL() 
{
	inherited::UpdateCL();
}

void CAntirad::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CAntirad::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CAntirad::renderable_Render() 
{
	if(getVisible() && !H_Parent()) 
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}