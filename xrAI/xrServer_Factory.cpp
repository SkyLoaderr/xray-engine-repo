////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Factory.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "clsid_game.h"
#include "xrServer_Objects_ALife_All.h"

CSE_Abstract *F_entity_Create			(LPCSTR caSection)
{
	if (!pSettings->section_exist(caSection))
		return							(0);
    
	CLASS_ID							cls = pSettings->r_clsid(caSection,"class");

	switch (cls) {
		case CLSID_OBJECT_ACTOR:		return xr_new<CSE_ALifeCreatureActor>			(caSection);          	// Visualed
		case CLSID_OBJECT_HLAMP:		return xr_new<CSE_ALifeObjectHangingLamp>		(caSection);            // Visualed
		
		case CLSID_LEVEL_POINT:			return xr_new<CSE_LevelPoint>					(caSection);
		case CLSID_AI_SPAWN_GROUP:		return xr_new<CSE_SpawnGroup>					(caSection);
		
		case CLSID_AI_GRAPH:			return xr_new<CSE_ALifeGraphPoint>				(caSection);			
		case CLSID_AI_CROW:				return xr_new<CSE_ALifeCreatureCrow>				(caSection);            // Visualed
		case CLSID_AI_RAT:				return xr_new<CSE_ALifeMonsterRat>				(caSection);			// Visualed
		case CLSID_AI_RAT_GROUP:		return xr_new<CSE_ALifeGroupTemplate<CSE_ALifeMonsterRat> >	(caSection);	
		case CLSID_AI_FLESH:			return xr_new<CSE_ALifeMonsterBiting>			(caSection);
		case CLSID_AI_FLESH_GROUP:		return xr_new<CSE_ALifeGroupTemplate<CSE_ALifeMonsterBiting> >(caSection);
		case CLSID_AI_CHIMERA:			return xr_new<CSE_ALifeMonsterBiting>			(caSection);
		case CLSID_AI_BOAR:				return xr_new<CSE_ALifeMonsterBiting>			(caSection);
		case CLSID_AI_BLOODSUCKER:		return xr_new<CSE_ALifeMonsterBiting>			(caSection);
		case CLSID_AI_ZOMBIE:			return xr_new<CSE_ALifeMonsterZombie>			(caSection);            // Visualed
		case CLSID_AI_DOG_RED:			return xr_new<CSE_ALifeMonsterBiting>			(caSection);			// Visualed
		case CLSID_AI_SOLDIER:			return xr_new<CSE_ALifeHumanStalker>			(caSection);           	// Visualed
		case CLSID_AI_STALKER:			return xr_new<CSE_ALifeHumanStalker>			(caSection);			// Visualed
		case CLSID_AI_IDOL:				return xr_new<CSE_ALifeObjectIdol>				(caSection);			// Visualed
		case CLSID_EVENT:				return xr_new<CSE_Event>						(caSection);
		case CLSID_CAR_NIVA:			return xr_new<CSE_ALifeItemCar>					(caSection);			// Visualed
		case CLSID_SPECTATOR:			return xr_new<CSE_Spectator>					(caSection);
		case CLSID_AI_TRADER:			return xr_new<CSE_ALifeTrader>					(caSection); 

		// Artifacts
//		case CLSID_AF_MERCURY_BALL:		return xr_new<xrSE_MercuryBall>					(caSection);
		case CLSID_ARTEFACT:			return xr_new<CSE_ALifeItemArtefact>			(caSection);

		// Zones
		case CLSID_ZONE:
		case CLSID_Z_MBALD:
		case CLSID_Z_ACIDF:
		case CLSID_Z_GALANT:
		case CLSID_Z_RADIO:
		case CLSID_Z_BFUZZ:
		case CLSID_Z_RUSTYH:
		case CLSID_Z_DEAD:
		case CLSID_Z_MINCER:			return xr_new<CSE_ALifeAnomalousZone>			(caSection);
		case CLSID_LEVEL_CHANGER:		return xr_new<CSE_ALifeLevelChanger>			(caSection);
		case CLSID_SCRIPT_ZONE:			return xr_new<CSE_ALifeScriptZone>				(caSection);

		// Detectors
		case CLSID_DETECTOR_SIMPLE:		return xr_new<CSE_ALifeItemDetector>			(caSection);
		case CLSID_DEVICE_PDA:			return xr_new<CSE_ALifeItemPDA>					(caSection);

		//Inventory items
		case CLSID_IITEM_ANTIRAD:
		case CLSID_IITEM_BREAD:
		case CLSID_IITEM_MEDKIT:		return xr_new<CSE_ALifeItem>					(caSection);
		//Info document
		case CLSID_IITEM_DOCUMENT:		return xr_new<CSE_ALifeItemDocument> 			(caSection);
		
		//Equipment outfit
		case CLSID_EQUIPMENT_SCIENTIFIC:
		case CLSID_EQUIPMENT_STALKER:	
		case CLSID_EQUIPMENT_MILITARY:	
		case CLSID_EQUIPMENT_EXO:		return xr_new<CSE_ALifeItem>					(caSection);



		case CLSID_OBJECT_W_M134:					
		case CLSID_OBJECT_W_FN2000:		
		case CLSID_OBJECT_W_AK74:		
		case CLSID_OBJECT_W_LR300:		
		case CLSID_OBJECT_W_HPSA:		
		case CLSID_OBJECT_W_PM:			
		case CLSID_OBJECT_W_VAL:		
		case CLSID_OBJECT_W_VINTOREZ:	
		case CLSID_OBJECT_W_WALTHER:	
		case CLSID_OBJECT_W_USP45:		
		case CLSID_OBJECT_W_GROZA:		
		case CLSID_OBJECT_W_FORT:		
		case CLSID_OBJECT_W_SHOTGUN:	
		case CLSID_OBJECT_W_SVD:		
		case CLSID_OBJECT_W_SVU:		
		case CLSID_OBJECT_W_RPG7:		
		case CLSID_OBJECT_W_KNIFE:		return xr_new<CSE_ALifeItemWeapon>				(caSection);  			// Visualed
		case CLSID_OBJECT_W_BINOCULAR:	return xr_new<CSE_ALifeItem>					(caSection);  			// Visualed
		case CLSID_OBJECT_A_PM:			return xr_new<CSE_ALifeItemAmmo>				(caSection);  			// Visualed
		case CLSID_OBJECT_G_RPG7:		return xr_new<CSE_Temporary>					(caSection);
		case CLSID_OBJECT_G_FAKE:		return xr_new<CSE_Temporary>					(caSection);

//		case CLSID_OBJECT_HEALTH:		return xr_new<xrSE_Health>						(caSection);
		case CLSID_TARGET_ASSAULT:		return xr_new<CSE_TargetAssault>				(caSection);
		case CLSID_TARGET_CS_BASE:		return xr_new<CSE_Target_CS_Base>				(caSection);
		case CLSID_TARGET_CS:			return xr_new<CSE_Target_CS>					(caSection);
		case CLSID_TARGET_CS_CASK:		return xr_new<CSE_Target_CS_Cask>				(caSection);
		case CLSID_IITEM_BOLT:			return xr_new<CSE_ALifeDynamicObject>			(caSection);
		case CLSID_GRENADE_F1:			return xr_new<CSE_ALifeItem>					(caSection);
		case CLSID_GRENADE_RGD5:		return xr_new<CSE_ALifeItem>					(caSection);
		case CLSID_DEVICE_TORCH:		return xr_new<CSE_ALifeItemTorch>				(caSection);			// Visualed
		case CLSID_OBJECT_PHYSIC:		return xr_new<CSE_ALifeObjectPhysic>			(caSection);            // Visualed

		default:						NODEFAULT;
	}
	
	return								(0);
}
