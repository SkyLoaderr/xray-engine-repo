#include "stdafx.h"
#include "ai_monster_jump.h"
#include "..\\custommonster.h"

CJumping::CJumping()
{
	active	= false;
}

void CJumping::Init(CCustomMonster *pM)
{
	pMonster	= pM;
	Reset		();
}

void CJumping::Reset()
{
	active				= false;
	time_next_allowed	= 0;
}

void CJumping::Load(LPCSTR section)
{
	m_fJumpFactor					= pSettings->r_float(section,"jump_factor");
	m_fJumpMinDist					= pSettings->r_float(section,"jump_min_dist");		
	m_fJumpMaxDist					= pSettings->r_float(section,"jump_max_dist");		
	m_fJumpMaxAngle					= pSettings->r_float(section,"jump_max_angle");
	m_dwDelayAfterJump				= pSettings->r_u32	(section,"jump_delay");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// NEW JUMPS
//////////////////////////////////////////////////////////////////////////////////////////////////////

void CJumping::AddState(CMotionDef *motion, EJumpStateType type, bool change, float linear, float angular)
{
	SJumpState jmp;

	jmp.motion			= motion;
	jmp.type			= type;
	jmp.change			= change;
	jmp.speed.linear	= linear;
	jmp.speed.angular	= angular;

	bank.push_back(jmp);
}

void CJumping::Start()
{
	ptr_cur		= bank.begin();
	cur_motion	= 0;
	active		= true;

	ApplyParams();
	if (ptr_cur->type == JT_GLIDE) Execute();

	Msg("Start jump...");
}

void CJumping::Stop()
{
	active = false;
	
	Msg("Stop jump...");
}
// вызывается на каждом SelectAnimation
bool CJumping::PrepareAnimation(CMotionDef **m)
{
	if (cur_motion != 0) return false;

	*m = cur_motion = ptr_cur->motion;
	return true;
}
// вызывается по окончанию анимации
void CJumping::OnAnimationEnd()
{
	if (ptr_cur->change) NextState();
}

void CJumping::ApplyParams()
{
	pMonster->m_fCurSpeed		= ptr_cur->speed.linear;
	pMonster->r_torso_speed		= ptr_cur->speed.angular;
	Msg("Apply params...");
}

void CJumping::NextState()
{
	Msg("Next state...");

	ptr_cur++;
	if (ptr_cur == bank.end()) {
		Stop();
		return;
	}

	cur_motion = 0;
	ApplyParams();

	if (ptr_cur->type == JT_GLIDE) Execute();
}

void CJumping::Execute()
{
	Msg("Execute Jump...");
	if (entity) {
		// установить целевую точку 
		u16 bone_id = PKinematics(entity->Visual())->LL_BoneID("bip01_head");
		CBoneInstance &bone = PKinematics(entity->Visual())->LL_GetBoneInstance(bone_id);

		Fmatrix global_transform;
		global_transform.set(entity->XFORM());
		global_transform.mulB(bone.mTransform);
		target_pos = global_transform.c;
		target_pos.y /= 1.5f;
	}

	// получить время физ.прыжка
	ph_time = pMonster->Movement.JumpMinVelTime(target_pos);
	// выполнить прыжок в соответствии с делителем времени
	pMonster->Movement.Jump(target_pos,ph_time/m_fJumpFactor);

	time_started		= pMonster->m_dwCurrentTime;
	time_next_allowed	= time_started + m_dwDelayAfterJump;
	
}

void CJumping::Update()
{
	if (!active) return;

	TTime itime = TTime(ph_time * 1000);

	// проверить на завершение прыжка
	if ((time_started + itime < pMonster->m_dwCurrentTime + TTime(itime/4))) {
		NextState();
	}
}

// Проверка на возможность прыжка
bool CJumping::Check(Fvector from_pos, Fvector to_pos, CObject *pO)
{
	if (!CanJump() || active || (time_next_allowed > pMonster->m_dwCurrentTime)) return false;

	// растояние до цели	
	float dist = from_pos.distance_to(to_pos);	

	// получить вектор направления и его мир угол
	Fvector dir;
	float  dest_yaw, pitch;
	dir.sub(to_pos, from_pos);
	dir.getHP(dest_yaw, pitch);
	dest_yaw *= -1;
	dest_yaw = angle_normalize(dest_yaw);

	// проверка на max_angle и на dist
	if (!getAI().bfTooSmallAngle(pMonster->r_torso_current.yaw, dest_yaw, m_fJumpMaxAngle) || !(m_fJumpMinDist <=dist && dist <= m_fJumpMaxDist)) return false;

	// можно прыгать; инициализировать параметры прыжка
	active			= true;

	target_yaw		= dest_yaw;
	this->from_pos	= from_pos;
	target_pos		= to_pos;
	entity			= pO;
	ph_time			= 0.f;

	pMonster->r_torso_target.yaw = target_yaw;
	pMonster->AI_Path.TravelPath.clear();

	Start();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// JUMPS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//// функция для UpdateCL
//void CMotionManager::JMP_Update()
//{
//	if (!jump.active) return;
//	
//	TTime itime = TTime(jump.ph_time * 1000);
//
//	// проверить на завершение прыжка
//	if ((jump.started + itime < pMonster->m_dwCurrentTime + TTime(itime/4)) && (jump.state == JS_JUMP)) {
//		JMP_Finish();
//	}else {
//		// tracing enemy here
//		if (jump.striked) return;
//		if (!jump.entity) return;
//		CEntity *pE = dynamic_cast<CEntity *>(jump.entity);
//		if (!pE) return;
//
//		Fvector trace_from;
//		pMonster->Center(trace_from);
//		pMonster->setEnabled(false);
//		Collide::ray_query	l_rq;
//
//		if (Level().ObjectSpace.RayPick(trace_from, pMonster->Direction(), jump.ptr_cur->trace_dist, l_rq)) {
//			if ((l_rq.O == jump.entity) && (l_rq.range < jump.ptr_cur->trace_dist)) {
//				pMonster->DoDamage(pE, pMonster->m_fHitPower,0,0);
//				jump.striked = true;
//			}
//		}
//		pMonster->setEnabled(true);			
//	}
//
//	if ((jump.state == JS_JUMP) && (jump.entity)) {
//		pMonster->LookPosition(jump.entity->Position());
//	}
//}

