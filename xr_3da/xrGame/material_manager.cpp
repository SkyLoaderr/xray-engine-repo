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

CMaterialManager::CMaterialManager	()
{
	Init					();
}

CMaterialManager::~CMaterialManager	()
{
}

void CMaterialManager::Init			()
{
	m_my_material_idx		= GAMEMTL_NONE_IDX;
}

void CMaterialManager::Load			(LPCSTR section)
{
	if (!pSettings->line_exist(section,"material")) {
		R_ASSERT3(false,"Material not found in the section ",*(dynamic_cast<CObject*>(this)->cNameSect()));
	}
	
	m_my_material_idx		= GMLib.GetMaterialIdx(pSettings->r_string(section,"material"));
}

void CMaterialManager::reinit		()
{
	m_last_material_idx		= GMLib.GetMaterialIdx("default");
	m_step_id				= 0;
	CEntityAlive			*entity_alive = dynamic_cast<CEntityAlive*>(this);
	if (entity_alive && entity_alive->m_PhysicMovementControl->CharacterExist()) {
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
	CObject					*object = dynamic_cast<CObject*>(this);
	VERIFY					(object);
	Fvector					position = object->Position();
	position.y				+= .10f; 
	// ref_sound step
	if (!standing) {
		if (m_time_to_step < 0) {
			m_step_id								= m_step_id ^ 1;
			m_time_to_step							= step_time;
			if (mtl_pair->StepSounds.size() >= 2){
				m_step_sound[m_step_id]				= mtl_pair->StepSounds[m_step_id];
				m_step_sound[m_step_id].play_at_pos	(object,position);
			}
		}
		m_time_to_step								-= time_delta;
	}
	else
		m_time_to_step								= 0;

	if (m_step_sound[0].feedback)		{
		m_step_sound[0].set_position	(position);
		m_step_sound[0].set_volume		(volume);
	}
	if (m_step_sound[1].feedback)		{
		m_step_sound[1].set_position	(position);
		m_step_sound[1].set_volume		(volume);
	}
}
