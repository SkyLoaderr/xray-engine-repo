////////////////////////////////////////////////////////////////////////////
//	Module 		: damage_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Damage manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "damage_manager.h"
#include "../xr_object.h"
#include "../skeletoncustom.h"

CDamageManager::CDamageManager			()
{
}

CDamageManager::~CDamageManager			()
{
}

DLL_Pure *CDamageManager::_construct	()
{
	m_object				= smart_cast<CObject*>(this);
	VERIFY					(m_object);
	return					(m_object);
}

void CDamageManager::reload				(LPCSTR section,CInifile* ini)
{
	m_default_hit_factor	= 1.f;
	m_default_wound_factor	= 1.f;

	//инициализировать default параметрами
	init_bones();
	if (ini->line_exist(section,"damage"))
		load_section(ini->r_string(section,"damage"),ini);
}
void CDamageManager::init_bones()
{
	CKinematics				*kinematics = smart_cast<CKinematics*>(m_object->Visual());
	VERIFY					(kinematics);
	for(u16 i = 0; i<kinematics->LL_BoneCount(); i++)
	{
		CBoneInstance			&bone_instance = kinematics->LL_GetBoneInstance(i);
		bone_instance.set_param	(0,1.f);
		bone_instance.set_param	(1,1.f);
		bone_instance.set_param	(2,1.f);
	}
}
void CDamageManager::load_section(LPCSTR section,CInifile* ini)
{
	string32				buffer;
	CKinematics				*kinematics = smart_cast<CKinematics*>(m_object->Visual());
	CInifile::Sect			&damages = ini->r_section(section);
	for (CInifile::SectIt i=damages.begin(); damages.end() != i; ++i) {
		if (!xr_strcmp(*(*i).first,"default"))
		{
			m_default_hit_factor	= (float)atof(_GetItem(*(*i).second,0,buffer));
			m_default_wound_factor  = (float)atof(_GetItem(*(*i).second,2,buffer));
		}
		else {
			VERIFY					(m_object);
			int						bone = kinematics->LL_BoneID(i->first);
			R_ASSERT2				(BI_NONE != bone, *(*i).first);
			CBoneInstance			&bone_instance = kinematics->LL_GetBoneInstance(u16(bone));
			bone_instance.set_param	(0,(float)atof(_GetItem(*(*i).second,0,buffer)));
			bone_instance.set_param	(1,(float)atoi(_GetItem(*(*i).second,1,buffer)));
			bone_instance.set_param	(2,(float)atof(_GetItem(*(*i).second,2,buffer)));
		}
	}
}


void  CDamageManager::HitScale			(const int element, float& hit_scale, float& wound_scale)
{
	if(BI_NONE == u16(element))
	{
		//считаем что параметры для BI_NONE заданы как 1.f 
		hit_scale = 1.f * m_default_hit_factor;
		wound_scale = 1.f * m_default_wound_factor;
		return;
	}

	CKinematics* V		= smart_cast<CKinematics*>(m_object->Visual());			VERIFY(V);
	float scale			= /*fis_zero(V->LL_GetBoneInstance(u16(element)).get_param(0))?1.f:*/V->LL_GetBoneInstance(u16(element)).get_param(0);
	//hit_scale			= m_default_hit_factor*scale;
	hit_scale			= scale;
	scale				= /*fis_zero(V->LL_GetBoneInstance(u16(element)).get_param(2))?1.f:*/V->LL_GetBoneInstance(u16(element)).get_param(2);
	//wound_scale			= m_default_wound_factor*scale;
	wound_scale			= scale;
}