#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBloodsuckerRest::CBloodsuckerRest(CAI_Bloodsucker *p)  
{
	pMonster = p;
	Reset();
}


void CBloodsuckerRest::Reset()
{
	IState::Reset();
	m_tAction			= ACTION_RUN;
}

void CBloodsuckerRest::Init()
{
	IState::Init();

	WRITE_TO_LOG("_ Blood State Init _");
}

void CBloodsuckerRest::Run()
{
	// ���������� ���������
	switch (m_tAction) {	
		case ACTION_RUN:

			if (!pMonster->Bones.isActive()) {
				//Look leftside/rightside

				bonesMotion		cur_motion;

				CBoneInstance *bone_one, *bone_two;
				int bone1	= PKinematics(pMonster->Visual())->LL_BoneID("bip01_spine");
				bone_one = &PKinematics(pMonster->Visual())->LL_GetInstance(bone1);
				int bone2	= PKinematics(pMonster->Visual())->LL_BoneID("bip01_head");
				bone_two = &PKinematics(pMonster->Visual())->LL_GetInstance(bone2);

				// �������� �����
				// ����� ��������� 3 ����
				cur_motion.AddBone(bone_one,AXIS_X, -PI_DIV_6, PI, 0, 0, 0, 0, 1000);
				cur_motion.AddBone(bone_two,AXIS_X, -PI_DIV_6, PI, 0, 0, 0, 0, 1000);

				pMonster->Bones.AddMotion(cur_motion);
				cur_motion.Clear();

				// �������� ������
				// ����� ��������� 3 ����
				cur_motion.AddBone(bone_one,AXIS_X, PI_DIV_6, PI, 0, 0, 0, 0, 1000);
				cur_motion.AddBone(bone_two,AXIS_X, PI_DIV_6, PI, 0, 0, 0, 0, 1000);

				pMonster->Bones.AddMotion(cur_motion);
			}

//			float newYaw;
//			newYaw = pMonster->r_torso_current.yaw;
//
//			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw + PI_DIV_4),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW, STOP_ANIM_END);
//			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw - PI_DIV_2),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW, STOP_AT_TURNED);
//			pMonster->Motion.m_tSeq.Switch();
//			
//			WRITE_TO_LOG("_ Switch _");

			break;
	}
}

bool CBloodsuckerRest::CheckCompletion()
{	
	return false;
}

