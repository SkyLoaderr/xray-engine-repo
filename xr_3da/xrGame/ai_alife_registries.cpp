////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_registries.cpp
//	Created 	: 15.01.2003
//  Modified 	: 15.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life registries
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

void CALifeObjectRegistry::Load(CStream	&tFileStream, CAI_ALife *tpALife)
{
	tFileStream.Read(&m_tObjectID,sizeof(m_tObjectID));
	u32 dwCount = tFileStream.Rdword();
	for (u32 i=0; i<dwCount; i++) {
		CALifeDynamicObject *tpALifeDynamicObject = 0;
		switch (tFileStream.Rbyte()) {
			case ALIFE_ITEM_ID : {
				tpALifeDynamicObject = new CALifeItem;
				break;
			}
			case ALIFE_MONSTER_ID : {
				tpALifeDynamicObject = new CALifeMonster;
				break;
			}
			case ALIFE_MONSTER_GROUP_ID : {
				tpALifeDynamicObject = new CALifeMonsterGroup;
				break;
			}
			case ALIFE_HUMAN_ID : {
				tpALifeDynamicObject = new CALifeHuman;
				break;
			}
			case ALIFE_HUMAN_GROUP_ID : {
				tpALifeDynamicObject = new CALifeHumanGroup;
				break;
			}
			default : NODEFAULT;
		};
		tpALifeDynamicObject->Load	(tFileStream);
		m_tppMap.insert			(make_pair(tpALifeDynamicObject->m_tObjectID,tpALifeDynamicObject));
		tpALife->vfUpdateDynamicData(tpALifeDynamicObject);
	}
}