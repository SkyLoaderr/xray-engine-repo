///////////////////////////////////////////////////////////////
// FadedBall.cpp
// FadedBall - артефакт блеклый шар
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FadedBall.h"
#include "PhysicsShell.h"


CFadedBall::CFadedBall(void) 
{
}

CFadedBall::~CFadedBall(void) 
{
}

void CFadedBall::Load(LPCSTR section) 
{
	inherited::Load(section);
}



void CFadedBall::UpdateCL() 
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