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

	string32				buffer;

	CKinematics				*kinematics = smart_cast<CKinematics*>(m_object->Visual());
	VERIFY					(kinematics);
	//инициализировать default параметрами
	for(u16 i = 0; i<kinematics->LL_BoneCount(); i++)
	{
		CBoneInstance			&bone_instance = kinematics->LL_GetBoneInstance(i);
		bone_instance.set_param	(0,1.f);
		bone_instance.set_param	(1,1.f);
		bone_instance.set_param	(2,1.f);
	}

	if (ini->line_exist(section,"damage")) {
		CInifile::Sect					&damages = ini->r_section(ini->r_string(section,"damage"));
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
}
