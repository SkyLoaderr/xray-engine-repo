///////////////////////////////////////////////////////////////
// Scope.cpp
// Scope - ������� ������ ����������� ������
///////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "scope.h"
#include "PhysicsShell.h"

CScope::CScope()
{
}

CScope::~CScope() 
{
}


BOOL CScope::net_Spawn(LPVOID DC) 
{
	return	(inherited::net_Spawn(DC));
}

void CScope::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CScope::net_Destroy() 
{
	inherited::net_Destroy();
}

void CScope::UpdateCL() 
{
	inherited::UpdateCL();
}

void CScope::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CScope::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
}

void CScope::renderable_Render() 
{
	inherited::renderable_Render();
}