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
}

void CDamageManager::Load				(LPCSTR section)
{
}

void CDamageManager::reinit				()
{
}

void CDamageManager::reload				(LPCSTR section)
{
	if (pSettings->line_exist(section,"damage")) {
		string32						buffer;
		CInifile::Sect					&damages = pSettings->r_section(pSettings->r_string(section,"damage"));
		for (CInifile::SectIt i=damages.begin(); damages.end() != i; ++i) {
			if (!xr_strcmp(*(*i).first,"default"))
				m_default_hit_factor	= (float)atof(*(*i).second);
			else {
				CObject					*object = dynamic_cast<CObject*>(this);
				VERIFY					(object);
				int						bone = PKinematics(object->Visual())->LL_BoneID(i->first); 
				Msg						("DAMAGE [%s][%s][%d][%f][%f]",*object->cNameSect(),*object->cNameVisual(),bone,(float)atof(_GetItem(*(*i).second,0,buffer)),(float)atof(_GetItem(*(*i).second,1,buffer)));
				R_ASSERT2				(BI_NONE != bone, *(*i).first);
				CBoneInstance			&bone_instance = PKinematics(object->Visual())->LL_GetBoneInstance(u16(bone));
				bone_instance.set_param	(0,(float)atof(_GetItem(*(*i).second,0,buffer)));
				bone_instance.set_param	(1,(float)atoi(_GetItem(*(*i).second,1,buffer)));
				float					p0 = bone_instance.get_param(0);
				float					p1 = bone_instance.get_param(0);
			}
		}
	}
}