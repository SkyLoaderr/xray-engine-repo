///////////////////////////////////////////////////////////////
// GraviArtifact.cpp
// GraviArtifact - �������������� ��������, ������� �� �����
// � ����������� ����� ��� ������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraviArtifact.h"
#include "PhysicsShell.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"

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
//	m_fEnergy = pSettings->r_float(section,"energy");
}



void CGraviArtifact::UpdateCL() 
{
	inherited::UpdateCL();

	if (getVisible() && m_pPhysicsShell) {
		if (m_fJumpHeight) {
			Fvector dir; 
			dir.set(0, -1.f, 0);
			Collide::rq_result RQ;
			setEnabled(false);
			
			//��������� ������ ���������
			if(Level().ObjectSpace.RayPick(Position(), dir, m_fJumpHeight, Collide::rqtBoth, RQ)) 
			{
				dir.y = 1.f; 
				m_pPhysicsShell->applyImpulse(dir, 
											  30.f * Device.fTimeDelta * 
											  m_pPhysicsShell->getMass());
			}
			setEnabled(true);
		}
	} else 
		if(H_Parent()) 
		{
			XFORM().set(H_Parent()->XFORM());
			
			if (Game().type == GAME_ARTEFACTHUNT && m_CarringBoneID != u16(-1))
			{
				CKinematics* K	= PKinematics(H_Parent()->Visual());
				if (K)
				{
					K->Calculate	();
					Fmatrix Ruck_MTX	= K->LL_GetTransform(m_CarringBoneID);
					Fvector	x;
					x.set(-0.1f, 0.f, -0.3f);
					Ruck_MTX.translate_add(x);
					Ruck_MTX.mulA		(XFORM());
					XFORM().set(Ruck_MTX);
				};
			};
		};
}