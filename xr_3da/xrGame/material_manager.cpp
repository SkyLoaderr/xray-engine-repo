////////////////////////////////////////////////////////////////////////////
//	Module 		: material_manager.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Material manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "material_manager.h"
#include "phmovementcontrol.h"
#include "entity_alive.h"

CMaterialManager::CMaterialManager	()
{
	init					();
}

CMaterialManager::~CMaterialManager	()
{
}

void CMaterialManager::init			()
{
	m_my_material_idx		= GAMEMTL_NONE_IDX;
	m_run_mode				= false;
	m_object				= 0;
}

void CMaterialManager::Load			(LPCSTR section)
{
	m_object				= dynamic_cast<CObject*>(this);
	VERIFY					(m_object);
	if (!pSettings->line_exist(section,"material")) {
		R_ASSERT3(false,"Material not found in the section ",*(m_object->cNameSect()));
	}
	
	m_my_material_idx		= GMLib.GetMaterialIdx(pSettings->r_string(section,"material"));
}

void CMaterialManager::reinit		()
{
	m_last_material_idx		= GMLib.GetMaterialIdx("default");
	m_step_id				= 0;
	m_run_mode				= false;

	CEntityAlive			*entity_alive = dynamic_cast<CEntityAlive*>(this);
	if (entity_alive ) {
		if(entity_alive->m_PhysicMovementControl->CharacterExist())
			entity_alive->m_PhysicMovementControl->SetPLastMaterial	(&m_last_material_idx);
		entity_alive->m_PhysicMovementControl->SetMaterial		(m_my_material_idx);
	}
}

void CMaterialManager::reload		(LPCSTR section)
{
}

void CMaterialManager::update		(float time_delta, float volume, float step_time, bool standing)
{
	VERIFY					(GAMEMTL_NONE_IDX != m_my_material_idx);
	VERIFY					(GAMEMTL_NONE_IDX != m_last_material_idx);
	SGameMtlPair			*mtl_pair = GMLib.GetMaterialPair(m_my_material_idx,m_last_material_idx);
	VERIFY3					(mtl_pair,"Undefined material pair: ", *GMLib.GetMaterialByIdx(m_last_material_idx)->m_Name);
	Fvector					position = m_object->Position();
	CPHMovementControl* movement=movement_control();
	if(movement&&movement->CharacterExist())
	{
		position.y				+= movement->FootRadius(); 
	}
	
	// ref_sound step
	if (!standing) {
		if (m_time_to_step < 0) {
			SoundSVec4& snd_array = mtl_pair->StepSounds;
			
			if(m_run_mode && mtl_pair->BreakingSounds.size() >0)
				snd_array = mtl_pair->BreakingSounds;

			if (snd_array.size() >0){
				m_step_id								= ::Random.randI(0, snd_array.size());
				m_time_to_step							= step_time;

				m_step_sound[m_step_id]					= snd_array[m_step_id];
				m_step_sound[m_step_id].play_at_pos	(m_object,position);
			}
		}
		m_time_to_step								-= time_delta;
	}
	else
		m_time_to_step								= 0;


	for(int i=0; i<4; i++)
		if (m_step_sound[i].feedback)		{
			m_step_sound[i].set_position	(position);
			m_step_sound[i].set_volume		(m_step_sound[i].handle->start_volume()*volume);
		}
}

void CMaterialManager::set_run_mode			(bool run_mode)
{
	m_run_mode			= run_mode;
}
