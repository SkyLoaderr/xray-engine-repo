///////////////////////////////////////////////////////////////
// GraviArtifact.cpp
// GraviArtifact - гравитационный артефакт, прыгает на месте
// и неустойчиво парит над землей
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraviArtifact.h"
#include "PhysicsShell.h"

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}


CGraviArtifact::CGraviArtifact(void) 
{
	shedule.t_min = 20;
	shedule.t_max = 50;
	
	m_fJumpHeight = 0;
	m_fEnergy = 1.f;
}

CGraviArtifact::~CGraviArtifact(void) 
{
}

void CGraviArtifact::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	if(pSettings->line_exist(section, "jump_height")) m_fJumpHeight = pSettings->r_float(section,"jump_height");
	m_fEnergy = pSettings->r_float(section,"energy");
}



void CGraviArtifact::UpdateCL() 
{
	inherited::UpdateCL();

	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set(m_pPhysicsShell->mXFORM);
		Position().set(m_pPhysicsShell->mXFORM.c);
		if(m_fJumpHeight) 
		{
			Fvector dir; 
			dir.set(0, -1.f, 0);
			Collide::ray_query RQ;
			setEnabled(false);
			
			//проверить высоту артифакта
			if(Level().ObjectSpace.RayPick(Position(), dir, m_fJumpHeight, RQ)) 
			{
				dir.y = 1.f; 
				m_pPhysicsShell->applyImpulse(dir, 
											  30.f * Device.fTimeDelta * 
											  m_pPhysicsShell->getMass());
			}
			setEnabled(true);
		}
	} else if(H_Parent()) XFORM().set(H_Parent()->XFORM());
}