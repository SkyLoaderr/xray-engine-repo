////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Factory.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma hdrstop

#include "clsid_game.h"
#include "xrServer_Objects_ALife_All.h"

CAbstractServerObject *F_entity_Create	(LPCSTR caSection)
{
	if (!pSettings->section_exist(caSection))
		return	(0);
    
	CLASS_ID	cls = pSettings->r_clsid(caSection,"class");

	switch (cls) {
		case CLSID_OBJECT_ACTOR:		return xr_new<CALifeCreatureActor>							(caSection);          	// Visualed
		case CLSID_OBJECT_DUMMY:		return xr_new<CServerObjectDummy>						(caSection);
		case CLSID_OBJECT_HLAMP:		return xr_new<CALifeObjectHangingLamp>					(caSection);            // Visualed
		case CLSID_AI_GRAPH:			return xr_new<CALifeGraphPoint>							(caSection);			
		case CLSID_AI_CROW:				return xr_new<CALifeMonsterCrow>						(caSection);            // Visualed
		case CLSID_AI_RAT:				return xr_new<CALifeMonsterRat>							(caSection);			// Visualed
		case CLSID_AI_RAT_GROUP:		return xr_new<CALifeGroupTemplate<CALifeMonsterRat> >	(caSection);	
		case CLSID_AI_FLESH:			return xr_new<CALifeMonsterBiting>						(caSection);
		case CLSID_AI_FLESH_GROUP:		return xr_new<CALifeGroupTemplate<CALifeMonsterBiting> >(caSection);
		case CLSID_AI_ZOMBIE:			return xr_new<CALifeMonsterZombie>						(caSection);            // Visualed
		case CLSID_AI_DOG:				return xr_new<CALifeMonsterBiting>						(caSection);			// Visualed
		case CLSID_AI_SOLDIER:			return xr_new<CALifeHumanStalker>						(caSection);           	// Visualed
		case CLSID_AI_STALKER:			return xr_new<CALifeHumanStalker>						(caSection);			// Visualed
		case CLSID_AI_IDOL:				return xr_new<CALifeObjectIdol>							(caSection);			// Visualed
		case CLSID_EVENT:				return xr_new<CServerObjectEvent>						(caSection);
		case CLSID_CAR_NIVA:			return xr_new<CALifeItemCar>							(caSection);			// Visualed
		case CLSID_SPECTATOR:			return xr_new<CServerObjectSpectator>					(caSection);

		// Artifacts
//		case CLSID_AF_MERCURY_BALL:		return xr_new<xrSE_MercuryBall>							(caSection);
		case CLSID_ARTEFACT:			return xr_new<CALifeItem>								(caSection);

		// Zones
		case CLSID_ZONE:				return xr_new<CALifeAnomalousZone>						(caSection);
		case CLSID_Z_MBALD:				return xr_new<CALifeAnomalousZone>						(caSection);
		case CLSID_Z_MINCER:			return xr_new<CALifeAnomalousZone>						(caSection);

		// Detectors
		case CLSID_DETECTOR_SIMPLE:		return xr_new<CALifeItemDetector>						(caSection);

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
		case CLSID_OBJECT_W_BINOCULAR:	
		case CLSID_OBJECT_W_SHOTGUN:	
		case CLSID_OBJECT_W_SVD:		
		case CLSID_OBJECT_W_SVU:		
		case CLSID_OBJECT_W_RPG7:		return xr_new<CALifeItemWeapon>							(caSection);  			// Visualed
		case CLSID_OBJECT_A_PM:			return xr_new<CALifeItemAmmo>							(caSection);  			// Visualed
		case CLSID_OBJECT_G_RPG7:		return xr_new<CServerObjectTemporary>					(caSection);
		case CLSID_OBJECT_G_FAKE:		return xr_new<CServerObjectTemporary>					(caSection);

//		case CLSID_OBJECT_HEALTH:		return xr_new<xrSE_Health>								(caSection);
		case CLSID_TARGET_ASSAULT:		return xr_new<CServerObjectTargetAssault>				(caSection);
		case CLSID_TARGET_CS_BASE:		return xr_new<CServerObjectTarget_CS_Base>				(caSection);
		case CLSID_TARGET_CS:			return xr_new<CServerObjectTarget_CS>					(caSection);
		case CLSID_TARGET_CS_CASK:		return xr_new<CServerObjectTarget_CS_Cask>				(caSection);
		case CLSID_IITEM_BOLT:			return xr_new<CALifeDynamicObject>						(caSection);
		case CLSID_GRENADE_F1:			return xr_new<CALifeItem>								(caSection);
		case CLSID_GRENADE_RGD5:		return xr_new<CALifeItem>								(caSection);
		case CLSID_DEVICE_TORCH:		return xr_new<CALifeItemTorch>							(caSection);			// Visualed
		case CLSID_OBJECT_PHYSIC:		return xr_new<CALifeObjectPhysic>							(caSection);            // Visualed

		default:						NODEFAULT;
	}
	
	return		(0);
}
