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

void CMotionManager::AA_Load(LPCSTR section)
{
	CHECK_SHARED_LOADED();

	if (!pSettings->section_exist(section)) return;

	SAAParam	anim;
	LPCSTR		anim_name,val;
	string16	cur_elem;

	for (u32 i=0; pSettings->r_line(section,i,&anim_name,&val); ++i) {
		_GetItem	(val,0,cur_elem);		anim.time			= float(atof(cur_elem));
		_GetItem	(val,1,cur_elem);		anim.hit_power		= float(atof(cur_elem));
		_GetItem	(val,2,cur_elem);		anim.impulse		= float(atof(cur_elem));
		_GetItem	(val,3,cur_elem);		anim.impulse_dir.x	= float(atof(cur_elem));
		_GetItem	(val,4,cur_elem);		anim.impulse_dir.y	= float(atof(cur_elem));
		_GetItem	(val,5,cur_elem);		anim.impulse_dir.z	= float(atof(cur_elem));
		_GetItem	(val,6,cur_elem);		anim.foh.from_yaw	= float(atof(cur_elem));
		_GetItem	(val,7,cur_elem);		anim.foh.to_yaw		= float(atof(cur_elem));
		_GetItem	(val,8,cur_elem);		anim.foh.from_pitch	= float(atof(cur_elem));
		_GetItem	(val,9,cur_elem);		anim.foh.to_pitch	= float(atof(cur_elem));
		_GetItem	(val,10,cur_elem);		anim.dist			= float(atof(cur_elem));

		anim.impulse_dir.normalize();
		_sd->aa_map.insert(mk_pair(anim_name, anim));
	}
}

void CMotionManager::STEPS_Load(LPCSTR section, u8 legs_num)
{
	CHECK_SHARED_LOADED();

	if (!pSettings->section_exist(section)) return;
	R_ASSERT((legs_num>=0) && (legs_num<=4));


	SStepParam	anim; 
	anim.step[0].time = 0.1f; // avoid warning

	LPCSTR		anim_name,val;
	string16	cur_elem;

	for (u32 i=0; pSettings->r_line(section,i,&anim_name,&val); ++i) {
		_GetItem (val,0,cur_elem);
		
		anim.cycles	= u8(atoi(cur_elem));
		R_ASSERT(anim.cycles >= 1);

		for (u32 j=0;j<legs_num;j++) {
			_GetItem	(val,1+j*2,cur_elem);		anim.step[j].time	= float(atof(cur_elem));
			_GetItem	(val,1+j*2+1,cur_elem);		anim.step[j].power	= float(atof(cur_elem));
		}
		_sd->steps_map.insert(mk_pair(anim_name, anim));
	}
}



