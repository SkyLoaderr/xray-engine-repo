///////////////////////////////////////////////////////////////
// ElectricBall.cpp
// ElectricBall - артефакт электрический шар
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ElectricBall.h"
#include "PhysicsShell.h"


CElectricBall::CElectricBall(void) 
{
}

CElectricBall::~CElectricBall(void) 
{
}

void CElectricBall::Load(LPCSTR section) 
{
	inherited::Load(section);
}



void CElectricBall::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);
	} 
	else if(H_Parent()) XFORM().set(H_Parent()->XFORM());
}