////////////////////////////////////////////////////////////////////////////
//	Module 		: damage_manager.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Damage manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "damage_manager.h"

CDamageManager::CDamageManager			()
{
	init								();
}

CDamageManager::~CDamageManager			()
{
}

void CDamageManager::init				()
{
	m_default_hit_factor				= 1.f;
	m_default_wound_factor				= 1.f;
}

void CDamageManager::Load				(LPCSTR section)
{
}

void CDamageManager::reinit				()
{
}

void CDamageManager::reload				(LPCSTR section)
{
	string32						buffer;

	CObject					*object = smart_cast<CObject*>(this);
	VERIFY					(object);

	//инициализировать default параметрами
	for(u16 i = 0; i<PKinematics(object->Visual())->LL_BoneCount(); i++)
	{
		CBoneInstance			&bone_instance = PKinematics(object->Visual())->LL_GetBoneInstance(i);
		bone_instance.set_param	(0,1.f);
		bone_instance.set_param	(1,-1);
		bone_instance.set_param	(2,1.f);
	}

	if (pSettings->line_exist(section,"damage")) {
		CInifile::Sect					&damages = pSettings->r_section(pSettings->r_string(section,"damage"));
		for (CInifile::SectIt i=damages.begin(); damages.end() != i; ++i) {
			if (!xr_strcmp(*(*i).first,"default"))
			{
                m_default_hit_factor	= (float)atof(_GetItem(*(*i).second,0,buffer));
				m_default_wound_factor  = (float)atof(_GetItem(*(*i).second,2,buffer));
			}
			else {
				VERIFY					(object);
				int						bone = PKinematics(object->Visual())->LL_BoneID(i->first);
				R_ASSERT2				(BI_NONE != bone, *(*i).first);
				CBoneInstance			&bone_instance = PKinematics(object->Visual())->LL_GetBoneInstance(u16(bone));
				bone_instance.set_param	(0,(float)atof(_GetItem(*(*i).second,0,buffer)));
				bone_instance.set_param	(1,(float)atoi(_GetItem(*(*i).second,1,buffer)));
				bone_instance.set_param	(2,(float)atof(_GetItem(*(*i).second,2,buffer)));
			}
		}
	}
}