#include "stdafx.h"
#include "step_manager.h"
#include "custommonster.h"

#define TIME_OFFSET 10

CStepManager::CStepManager()
{
}

CStepManager::~CStepManager()
{
}

void CStepManager::load(LPCSTR section)
{	
	m_legs_count		= pSettings->r_u8		(section, "LegsCount");
	LPCSTR anim_section = pSettings->r_string	(section, "step_params");
	
	if (!pSettings->section_exist(anim_section)) return;
	VERIFY((m_legs_count>=MIN_LEGS_COUNT) && (m_legs_count<=MAX_LEGS_COUNT));

	SStepParam	param; 
	param.step[0].time = 0.1f;	// avoid warning

	LPCSTR		anim_name, val;
	string16	cur_elem;

	for (u32 i=0; pSettings->r_line(anim_section,i,&anim_name,&val); ++i) {
		_GetItem (val,0,cur_elem);

		param.cycles = u8(atoi(cur_elem));
		R_ASSERT(param.cycles >= 1);

		for (u32 j=0;j<m_legs_count;j++) {
			_GetItem	(val,1+j*2,		cur_elem);		param.step[j].time	= float(atof(cur_elem));
			_GetItem	(val,1+j*2+1,	cur_elem);		param.step[j].power	= float(atof(cur_elem));
		}
		m_steps_map.insert(mk_pair(anim_name, param));
	}
}

void CStepManager::reinit()
{
	for (u32 i = 0; i < MAX_LEGS_COUNT; i++) m_foot_bones[i] = BI_NONE;

	reload_foot_bones	();
	
	m_time_anim_started	= 0;
}

void CStepManager::on_animation_start(shared_str anim)
{
	m_time_anim_started = Level().timeServer(); 

	// искать текущую анимацию в STEPS_MAP
	STEPS_MAP_IT it = m_steps_map.find(anim);
	if (it == m_steps_map.end()) {
		m_step_info.disable = true;
		return;
	}

	m_step_info.disable		= false;
	m_step_info.params		= it->second;
	m_step_info.cur_cycle	= 1;					// all cycles are 1-based

	for (u32 i=0; i<MAX_LEGS_COUNT; i++) {
		m_step_info.activity[i].handled	= false;
		m_step_info.activity[i].cycle	= m_step_info.cur_cycle;
	}
}


void CStepManager::update()
{
	if (m_step_info.disable) return;

	SGameMtlPair* mtl_pair	= m_object->CMaterialManager::get_current_pair();
	if (!mtl_pair) return;

	// получить параметры шага
	SStepParam	&step		= m_step_info.params;
	TTime		cur_time	= Level().timeServer();

	// время одного цикла анимации
	float cycle_anim_time	= get_current_animation_time() / step.cycles;

	for (u32 i=0; i<m_legs_count; i++) {

		// если событие уже обработано для этой ноги, то skip
		if (m_step_info.activity[i].handled && (m_step_info.activity[i].cycle == m_step_info.cur_cycle)) continue;

		// вычислить смещённое время шага в соответствии с параметрами анимации ходьбы
		TTime offset_time = m_time_anim_started + u32(1000 * (cycle_anim_time * (m_step_info.cur_cycle-1) + cycle_anim_time * step.step[i].time));

		if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){

			// Играть звук
			if (!mtl_pair->StepSounds.empty()) {

				Fvector sound_pos = m_object->Position();
				sound_pos.y += 0.5;

				SELECT_RANDOM(m_step_info.activity[i].sound, mtl_pair, StepSounds);
				m_step_info.activity[i].sound.play_at_pos	(m_object, sound_pos);

			}

			// Играть партиклы
			if (!mtl_pair->CollideParticles.empty()) {
				LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

				//отыграть партиклы столкновения материалов
				CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

				// вычислить позицию и направленность партикла
				Fmatrix pos; 

				// установить направление
				pos.k.set(Fvector().set(0.0f,1.0f,0.0f));
				Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);

				// установить позицию
				pos.c.set(get_foot_position(ELegType(i)));

				ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
				Level().ps_needtoplay.push_back(ps);
			}

			// Play Camera FXs
			event_on_step();

			// обновить поле handle
			m_step_info.activity[i].handled	= true;
			m_step_info.activity[i].cycle	= m_step_info.cur_cycle;
		}
	}

	// определить текущий цикл
	if (m_step_info.cur_cycle < step.cycles) m_step_info.cur_cycle = 1 + u8(float(cur_time - m_time_anim_started) / (1000.f * cycle_anim_time));

	// позиционировать играемые звуки
	for (i=0; i<m_legs_count; i++) {
		if (m_step_info.activity[i].handled && m_step_info.activity[i].sound.feedback) {
			Fvector sound_pos = m_object->Position();
			sound_pos.y += 0.5;
			m_step_info.activity[i].sound.set_position	(sound_pos);
			//m_step_info.activity[i].sound.set_volume	(m_step_info.params.step[i].power);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Function for foot processing
//////////////////////////////////////////////////////////////////////////
Fvector	CStepManager::get_foot_position(ELegType leg_type)
{
	R_ASSERT2(m_foot_bones[leg_type] != BI_NONE, "foot bone had not been set");

	CKinematics *pK = PKinematics(m_object->Visual());
	Fmatrix bone_transform;

	bone_transform = pK->LL_GetBoneInstance(m_foot_bones[leg_type]).mTransform;	

	Fmatrix global_transform;
	global_transform.set(m_object->XFORM());
	global_transform.mulB(bone_transform);

	return global_transform.c;
}

void CStepManager::reload_foot_bones()
{
	CInifile* ini = PKinematics(m_object->Visual())->LL_UserData();
	if(ini&&ini->section_exist("foot_bones")){

		CInifile::Sect& data = ini->r_section("foot_bones");
		for (CInifile::SectIt I=data.begin(); I!=data.end(); I++){
			CInifile::Item& item	= *I;

			u16 index = PKinematics(m_object->Visual())->LL_BoneID(*item.second);
			VERIFY3(index != BI_NONE, "foot bone not found", *item.second);

			if (xr_strcmp(*item.first, "front_left") == 0) 			m_foot_bones[eFrontLeft]	= index;
			else if (xr_strcmp(*item.first, "front_right")== 0)		m_foot_bones[eFrontRight]	= index;
			else if (xr_strcmp(*item.first, "back_right")== 0)		m_foot_bones[eBackRight]	= index;
			else if (xr_strcmp(*item.first, "back_left")== 0)		m_foot_bones[eBackLeft]		= index;
		}
	} else VERIFY("section [foot_bones] not found in monster user_data");

	// проверка на соответсвие
	int count = 0;
	for (u32 i = 0; i < MAX_LEGS_COUNT; i++) 
		if (m_foot_bones[i] != BI_NONE) count++;

	VERIFY(count == m_legs_count);
}

