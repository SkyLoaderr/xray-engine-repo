#include "stdafx.h"
#include "ai_monster_motion.h"
#include "biting\ai_biting.h"

// Загрузка параметров анимации. Вызывать необходимо на Monster::Load
void CMotionManager::AddAnim(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s, LPCSTR fx_front, LPCSTR fx_back, LPCSTR fx_left, LPCSTR fx_right)
{
	CHECK_SHARED_LOADED();

	SAnimItem new_item;

	new_item.target_name	= tn;
	new_item.spec_id		= s_id;
	new_item.velocity		= vel;
	new_item.pos_state		= p_s;

	new_item.fxs.front		= fx_front;
	new_item.fxs.back		= fx_back;
	new_item.fxs.left		= fx_left;
	new_item.fxs.right		= fx_right;

	_sd->m_tAnims.insert			(mk_pair(ma, new_item));
}

// Загрузка параметров анимации. Вызывать необходимо на Monster::Load
void CMotionManager::AddAnim(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s)
{
	CHECK_SHARED_LOADED();

	SAnimItem new_item;

	new_item.target_name	= tn;
	new_item.spec_id		= s_id;
	new_item.velocity		= vel;
	new_item.pos_state		= p_s;

	_sd->m_tAnims.insert			(mk_pair(ma, new_item));
}


// Загрузка анимаций. Необходимо вызывать на Monster::NetSpawn 
void CMotionManager::LoadVisualData()
{
	for (ANIM_ITEM_MAP_IT item_it = _sd->m_tAnims.begin(); _sd->m_tAnims.end() != item_it; ++item_it) {
		// Очистка старых анимаций
		if (!item_it->second.pMotionVect.empty()) item_it->second.pMotionVect.clear();
		// Загрузка новых
		Load(*item_it->second.target_name, &item_it->second.pMotionVect);
	}
}

void CMotionManager::AddTransition(EMotionAnim from, EMotionAnim to, EMotionAnim trans, bool chain)
{
	STransition new_item;

	new_item.from.state_used	= false;
	new_item.from.anim			= from;

	new_item.target.state_used	= false;
	new_item.target.anim		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}


void CMotionManager::AddTransition(EMotionAnim from, EPState to, EMotionAnim trans, bool chain)
{
	CHECK_SHARED_LOADED();

	STransition new_item;

	new_item.from.state_used	= false;
	new_item.from.anim			= from;

	new_item.target.state_used	= true;
	new_item.target.state		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}

void CMotionManager::AddTransition(EPState from, EMotionAnim to, EMotionAnim trans, bool chain)
{
	CHECK_SHARED_LOADED();

	STransition new_item;

	new_item.from.state_used	= true;
	new_item.from.state			= from;

	new_item.target.state_used	= false;
	new_item.target.anim		= to;


	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}

void CMotionManager::AddTransition(EPState from, EPState to, EMotionAnim trans, bool chain)
{
	CHECK_SHARED_LOADED();

	STransition new_item;

	new_item.from.state_used	= true;
	new_item.from.state			= from;

	new_item.target.state_used	= true;
	new_item.target.state		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	_sd->m_tTransitions.push_back(new_item);
}

void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle)
{
	CHECK_SHARED_LOADED();

	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= true;
	new_item.turn.anim_left		= pmt_left;
	new_item.turn.anim_right	= pmt_right;
	new_item.turn.min_angle		= pmt_angle;

	_sd->m_tMotions.insert	(mk_pair(act, new_item));
}

void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion)
{
	CHECK_SHARED_LOADED();

	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= false;

	_sd->m_tMotions.insert	(mk_pair(act, new_item));
}

void CMotionManager::AddReplacedAnim(bool *b_flag, EMotionAnim pmt_cur_anim, EMotionAnim pmt_new_anim)
{
	SReplacedAnim ra;

	ra.flag		= b_flag;
	ra.cur_anim = pmt_cur_anim;
	ra.new_anim = pmt_new_anim;

	m_tReplacedAnims.push_back(ra);
}


// загрузка анимаций из модели начинающиеся с pmt_name в вектор pMotionVect
void CMotionManager::Load(LPCTSTR pmt_name, ANIM_VECTOR	*pMotionVect)
{

	string256	S1, S2; 
	CMotionDef	*tpMotionDef;
	for (int i=0; ; ++i) {
		if (0 != (tpMotionDef = PSkeletonAnimated(pVisual)->ID_Cycle_Safe(strconcat(S1,pmt_name,itoa(i,S2,10)))))  pMotionVect->push_back(tpMotionDef);
		else if (0 != (tpMotionDef = PSkeletonAnimated(pVisual)->ID_FX_Safe(strconcat(S1,pmt_name,itoa(i,S2,10))))) pMotionVect->push_back(tpMotionDef);
		else {
			if (i == 0) {
				string128	s;
				sprintf(s, "Error! No animation: %s for monster %s", pmt_name, *pMonster->cName());
				R_ASSERT2(i != 0, s);
			}
			break;
		}
	}
}

void CMotionManager::AA_PushAttackAnim(SAttackAnimation AttackAnim)
{	
	_sd->aa_all.push_back(AttackAnim);
}

void CMotionManager::AA_PushAttackAnim(EMotionAnim a, u32 i3, TTime from, TTime to, const Fvector &ray_from, const Fvector &ray_to, float damage, Fvector &dir, u32 flags)
{
	CHECK_SHARED_LOADED();	

	SAttackAnimation anim;
	anim.anim			= a;
	anim.anim_i3		= i3;

	anim.time_from		= from;
	anim.time_to		= to;

	anim.trace_from		= ray_from;
	anim.trace_to		= ray_to;

	anim.damage			= damage;
	anim.hit_dir			= dir;

	anim.flags			= flags;

	AA_PushAttackAnim	(anim);
}

void CMotionManager::AA_PushAttackAnimTest(EMotionAnim a, u32 i3, TTime from, TTime to, float y1, float y2, float p1, float p2, float dist, float damage, Fvector &dir, u32 flags)
{
	CHECK_SHARED_LOADED();	

	SAttackAnimation anim;
	anim.anim			= a;
	anim.anim_i3		= i3;

	anim.time_from		= from;
	anim.time_to		= to;

	anim.damage			= damage;
	anim.hit_dir		= dir;

	anim.flags			= flags;

	anim.yaw_from		= y1;
	anim.yaw_to			= y2;
	anim.pitch_from		= p1;
	anim.pitch_to		= p2;
	anim.dist			= dist;

	AA_PushAttackAnim	(anim);
}

