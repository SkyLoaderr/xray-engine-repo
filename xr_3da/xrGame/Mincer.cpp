#include "stdafx.h"
#include "mincer.h"
#include "hudmanager.h"
#include "xrmessages.h"
#include "level.h"

CMincer::CMincer(void) 
{
}

CMincer::~CMincer(void) 
{
}

void CMincer::Load (LPCSTR section)
{
	inherited::Load(section);
	//pSettings->r_fvector3(section,whirlwind_center);
}

BOOL CMincer::net_Spawn(LPVOID DC)
{
	BOOL result=inherited::net_Spawn(DC);
	Fvector C;
	Center(C);
	m_telekinetics.SetCenter(C);
	return result;
}