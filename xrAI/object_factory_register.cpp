////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_register.cpp
//	Created 	: 27.05.2004
//  Modified 	: 27.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "object_factory_impl.h"

// server entities includes
#include "xrServer_Objects_ALife_All.h"

// client entities includes
#ifdef NO_XR_GAME
#	include "clsid_game.h"
#else
#	include "level.h"
#	include "gamepersistent.h"
#	include "hudmanager.h"
#	include "actor.h"
#	include "customtarget.h"
#	include "spectator.h"

#	include "ai/rat/ai_rat.h"
#	include "ai/flesh/ai_flesh.h"
#	include "ai/monsters/chimera/chimera.h"
#	include "ai/dog/ai_dog.h"
#	include "ai/stalker/ai_stalker.h"
#	include "ai/idol/ai_idol.h"
#	include "ai/bloodsucker/ai_bloodsucker.h"
#	include "ai/boar/ai_boar.h"
#	include "ai/pseudodog/ai_pseudodog.h"
#	include "ai/monsters/Burer/burer.h"
#	include "ai/monsters/PseudoGigant/pseudo_gigant.h"
#	include "ai/monsters/controller/controller.h"
#	include "ai/monsters/poltergeist/poltergeist.h"
#	include "ai/monsters/zombie/zombie.h"
#	include "ai/monsters/fracture/fracture.h"

#	include "ai/trader/ai_trader.h"

#	include "ai/crow/ai_crow.h"
#	include "car.h"

#	include "helicopter.h"

#	include "MercuryBall.h"
#	include "BlackDrops.h"
#	include "BlackGraviArtifact.h"
#	include "BastArtifact.h"
#	include "DummyArtifact.h"
#	include "ZudaArtifact.h"
#	include "ThornArtifact.h"
#	include "FadedBall.h"
#	include "ElectricBall.h"
#	include "RustyHairArtifact.h"
#	include "GalantineArtifact.h"
#	include "GraviArtifact.h"

#	include "targetassault.h"
#	include "targetcsbase.h"
#	include "targetcs.h"
#	include "targetcscask.h"

#	include "weaponFN2000.h"
#	include "weaponAK74.h"
#	include "weaponLR300.h"
#	include "weaponHPSA.h"
#	include "weaponPM.h"
#	include "weaponAMMO.h"
#	include "weaponFORT.h"
#	include "weaponBINOCULARS.h"
#	include "weaponShotgun.h"
#	include "weaponsvd.h"
#	include "weaponsvu.h"
#	include "weaponrpg7.h"
#	include "weaponval.h"
#	include "weaponvintorez.h"
#	include "weaponwalther.h"
#	include "weaponusp45.h"
#	include "weapongroza.h"
#	include "weaponknife.h"

#	include "scope.h"
#	include "silencer.h"
#	include "grenadelauncher.h"

#	include "bolt.h"
#	include "medkit.h"
#	include "antirad.h"
#	include "fooditem.h"
#	include "bottleitem.h"
#	include "explosiveitem.h"

#	include "infodocument.h"
#	include "attachable_item.h"

#	include "ScientificOutfit.h"
#	include "StalkerOutfit.h"
#	include "MilitaryOutfit.h"
#	include "ExoOutfit.h"

#	include "f1.h"
#	include "rgd5.h"

#	include "explosiverocket.h"

#	include "customzone.h"
#	include "mosquitobald.h"
#	include "mincer.h"
#	include "gravizone.h"
#	include "radioactivezone.h"
#	include "level_changer.h"
#	include "script_zone.h"
#	include "team_base_zone.h"
#	include "torridZone.h"

#	include "simpledetector.h"

#	include "torch.h"
#	include "pda.h"
#	include "artifactmerger.h"

#	include "searchlight.h"
#	include "WeaponMounted.h"

#	include "HangingLamp.h"
#	include "physicobject.h"
#	include "ai/script/ai_script_monster.h"
#	include "BreakableObject.h"
#	include "PhysicsSkeletonObject.h"

#	include "game_sv_single.h"
#	include "game_sv_deathmatch.h"
#	include "game_sv_teamdeathmatch.h"
#	include "game_sv_ArtefactHunt.h"

#	include "game_cl_single.h"
#	include "game_cl_deathmatch.h"
#	include "game_cl_teamdeathmatch.h"
#	include "game_cl_ArtefactHunt.h"

#	include "UIGameSP.h"
#	include "UIGameAHunt.h"

#	include "space_restrictor.h"
#endif

#ifndef NO_XR_GAME
#	define ADD(a,b,c,d) add<a,b>(c,d)
#else
#	define ADD(a,b,c,d) add<b>(c,d)
#endif

void CObjectFactory::register_classes	()
{
#ifndef NO_XR_GAME
	// client entities
	add<CLevel>													(CLSID_GAME_LEVEL			,"level");
	add<CGamePersistent>										(CLSID_GAME_PERSISTANT		,"game");
	add<CHUDManager>											(CLSID_HUDMANAGER			,"hud_manager");
	add<CCustomTarget>											(CLSID_TARGET				,"custom_target");

	//Server Game type
	add<game_sv_Single>											(CLSID_SV_GAME_SINGLE			,"game_sv_single");
	add<game_sv_Deathmatch>										(CLSID_SV_GAME_DEATHMATCH		,"game_sv_deathmatch");
	add<game_sv_TeamDeathmatch>									(CLSID_SV_GAME_TEAMDEATHMATCH	,"game_sv_team_deathmatch");
	add<game_sv_ArtefactHunt>									(CLSID_SV_GAME_ARTEFACTHUNT		,"game_sv_artefact_hunt");
	//Client Game type
	add<game_cl_Single>											(CLSID_CL_GAME_SINGLE			,"game_cl_single");
	add<game_cl_Deathmatch>										(CLSID_CL_GAME_DEATHMATCH		,"game_cl_deathmatch");
	add<game_cl_TeamDeathmatch>									(CLSID_CL_GAME_TEAMDEATHMATCH	,"game_cl_team_deathmatch");
	add<game_cl_ArtefactHunt>									(CLSID_CL_GAME_ARTEFACTHUNT		,"game_cl_artefact_hunt");

	//Game UI type
	add<CUIGameSP>												(CLSID_GAME_UI_SINGLE			,"game_ui_single");
	add<CUIGameDM>												(CLSID_GAME_UI_DEATHMATCH		,"game_ui_deathmatch");
	add<CUIGameTDM>												(CLSID_GAME_UI_TEAMDEATHMATCH	,"game_ui_team_deathmatch");
	add<CUIGameAHunt>											(CLSID_GAME_UI_ARTEFACTHUNT		,"game_ui_artefact_hunt");

#endif

	// server entities
	add<CSE_ALifeGroupTemplate<CSE_ALifeMonsterRat>	>			(CLSID_AI_RAT_GROUP			,"rat_group");
	add<CSE_ALifeGroupTemplate<CSE_ALifeMonsterBiting> >		(CLSID_AI_FLESH_GROUP		,"flesh_group");
	add<CSE_SpawnGroup>											(CLSID_AI_SPAWN_GROUP		,"spawn_group");
	add<CSE_Event>												(CLSID_EVENT				,"event");
	add<CSE_ALifeGraphPoint>									(CLSID_AI_GRAPH				,"graph_point");

	// client and server entities
	ADD(CActor					,CSE_ALifeCreatureActor			,CLSID_OBJECT_ACTOR			,"actor");
	ADD(CSpectator				,CSE_Spectator					,CLSID_SPECTATOR			,"spectator");

	ADD(CAI_Rat					,CSE_ALifeMonsterRat			,CLSID_AI_RAT				,"rat");
	ADD(CAI_Flesh				,CSE_ALifeMonsterBiting			,CLSID_AI_FLESH				,"flesh");
	ADD(CChimera				,CSE_ALifeMonsterBiting			,CLSID_AI_CHIMERA			,"chimera");
	ADD(CAI_Dog					,CSE_ALifeMonsterBiting			,CLSID_AI_DOG_RED			,"dog_red");
	ADD(CAI_Stalker				,CSE_ALifeHumanStalker			,CLSID_AI_SOLDIER			,"soldier");
	ADD(CAI_Stalker				,CSE_ALifeHumanStalker			,CLSID_AI_STALKER			,"stalker");
	ADD(CAI_Idol				,CSE_ALifeObjectIdol			,CLSID_AI_IDOL				,"idol");
	ADD(CAI_Bloodsucker			,CSE_ALifeMonsterBiting			,CLSID_AI_BLOODSUCKER		,"bloodsucker");
	ADD(CAI_Boar				,CSE_ALifeMonsterBiting			,CLSID_AI_BOAR				,"boar");
	ADD(CAI_PseudoDog			,CSE_ALifeMonsterBiting			,CLSID_AI_DOG_BLACK			,"dog_black");
	ADD(CBurer					,CSE_ALifeMonsterBiting			,CLSID_AI_BURER				,"burer");
	ADD(CPseudoGigant			,CSE_ALifeMonsterBiting			,CLSID_AI_GIANT				,"pseudo_gigant");
	ADD(CController				,CSE_ALifeMonsterBiting			,CLSID_AI_CONTROLLER		,"controller");
	ADD(CPoltergeist			,CSE_ALifeMonsterBiting			,CLSID_AI_PHANTOM			,"poltergeist");
	ADD(CZombie					,CSE_ALifeMonsterBiting			,CLSID_AI_ZOMBIE			,"zombie");
	ADD(CFracture				,CSE_ALifeMonsterBiting			,CLSID_AI_FRACTURE			,"fracture");


	// Trader
	ADD(CAI_Trader				,CSE_ALifeTrader				,CLSID_AI_TRADER			,"trader");

	ADD(CAI_Crow				,CSE_ALifeCreatureCrow			,CLSID_AI_CROW				,"crow");
	ADD(CCar					,CSE_ALifeCar					,CLSID_CAR					,"car");

	ADD(CHelicopter				,CSE_ALifeHelicopter			,CLSID_VEHICLE_HELICOPTER	,"helicopter");

	// Artefacts
	ADD(CMercuryBall			,CSE_ALifeItemArtefact			,CLSID_AF_MERCURY_BALL		,"art_mercury_ball");
	ADD(CBlackDrops				,CSE_ALifeItemArtefact			,CLSID_AF_BLACKDROPS		,"art_black_drops");
	ADD(CBlackGraviArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_NEEDLES			,"art_needles");
	ADD(CBastArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_BAST				,"art_bast_artefact");
	ADD(CBlackGraviArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_BLACK_GRAVI		,"art_gravi_black");
	ADD(CDummyArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_DUMMY				,"art_dummy");
	ADD(CZudaArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_ZUDA				,"art_zuda");
	ADD(CThornArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_THORN				,"art_thorn");
	ADD(CFadedBall				,CSE_ALifeItemArtefact			,CLSID_AF_FADED_BALL		,"art_faded_ball");
	ADD(CElectricBall			,CSE_ALifeItemArtefact			,CLSID_AF_ELECTRIC_BALL		,"art_electric_ball");
	ADD(CRustyHairArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_RUSTY_HAIR		,"art_rusty_hair");
	ADD(CGalantineArtefact		,CSE_ALifeItemArtefact			,CLSID_AF_GALANTINE			,"art_galantine");
	ADD(CGraviArtefact			,CSE_ALifeItemArtefact			,CLSID_AF_GRAVI				,"art_gravi");
	ADD(CGraviArtefact			,CSE_ALifeItemArtefact			,CLSID_ARTEFACT				,"artefact");

	// Targets
	ADD(CTargetAssault			,CSE_TargetAssault				,CLSID_TARGET_ASSAULT		,"net_target_assault");
	ADD(CTargetCSBase			,CSE_Target_CS_Base				,CLSID_TARGET_CS_BASE		,"net_target_cs_base");
	ADD(CTargetCSCask			,CSE_Target_CS_Cask				,CLSID_TARGET_CS_CASK		,"net_target_cs_cask");
	ADD(CTargetCS				,CSE_Target_CS					,CLSID_TARGET_CS			,"net_target_cs");
	
	ADD(CWeaponFN2000			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_FN2000		,"wpn_fn2000");
	ADD(CWeaponAK74				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_AK74		,"wpn_ak74");
	ADD(CWeaponLR300			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_LR300		,"wpn_lr300");
	ADD(CWeaponHPSA				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_HPSA		,"wpn_hpsa");
	ADD(CWeaponPM				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_PM			,"wpn_pm");
	ADD(CWeaponFORT				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_FORT		,"wpn_fort");
	ADD(CWeaponBinoculars		,CSE_ALifeItem					,CLSID_OBJECT_W_BINOCULAR	,"wpn_binocular");
	ADD(CWeaponShotgun			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_SHOTGUN		,"wpn_shotgun");
	ADD(CWeaponSVD				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_SVD			,"wpn_svd");
	ADD(CWeaponSVU				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_SVU			,"wpn_svu");
	ADD(CWeaponRPG7				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_RPG7		,"wpn_rpg7");
	ADD(CWeaponVal				,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_VAL			,"wpn_val");
	ADD(CWeaponVintorez			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_VINTOREZ	,"wpn_vintorez");
	ADD(CWeaponWalther			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_WALTHER		,"wpn_walther");
	ADD(CWeaponUSP45			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_USP45		,"wpn_usp45");
	ADD(CWeaponGroza			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_GROZA		,"wpn_groza");
	ADD(CWeaponKnife			,CSE_ALifeItemWeapon			,CLSID_OBJECT_W_KNIFE		,"wpn_knife");
	//-----------------------------------------------------------------------------------------------------
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_A_PM			,"wpn_ammo");
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_A_VOG25		,"wpn_ammo_vog25");
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_A_OG7B		,"wpn_ammo_og7b");
	ADD(CWeaponAmmo				,CSE_ALifeItemAmmo				,CLSID_OBJECT_A_M209		,"wpn_ammo_m209");
	//-----------------------------------------------------------------------------------------------------

	//Weapons Add-on
	ADD(CScope					,CSE_ALifeItem					,CLSID_OBJECT_W_SCOPE		,"wpn_scope");
	ADD(CSilencer				,CSE_ALifeItem					,CLSID_OBJECT_W_SILENCER	,"wpn_silencer");
	ADD(CGrenadeLauncher		,CSE_ALifeItem					,CLSID_OBJECT_W_GLAUNCHER	,"wpn_grenade_launcher");

	// Inventory
	ADD(CBolt					,CSE_ALifeItemBolt				,CLSID_IITEM_BOLT			,"obj_bolt");
	ADD(CMedkit					,CSE_ALifeItem					,CLSID_IITEM_MEDKIT			,"obj_medkit");
	ADD(CAntirad				,CSE_ALifeItem					,CLSID_IITEM_ANTIRAD		,"obj_antirad");
	ADD(CFoodItem				,CSE_ALifeItem					,CLSID_IITEM_FOOD			,"obj_food");
	ADD(CBottleItem				,CSE_ALifeItem					,CLSID_IITEM_BOTTLE			,"obj_bottle");
	ADD(CExplosiveItem			,CSE_ALifeItemExplosive			,CLSID_IITEM_EXPLOSIVE		,"obj_explosive");
	
	//Info Document
	ADD(CInfoDocument			,CSE_ALifeItemDocument			,CLSID_IITEM_DOCUMENT		,"obj_document");
	ADD(CAttachableItem			,CSE_ALifeItem					,CLSID_IITEM_ATTACH			,"obj_attachable");

	//Equipment outfit
	ADD(CScientificOutfit		,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_SCIENTIFIC	,"equ_scientific");
	ADD(CStalkerOutfit			,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_STALKER	,"equ_stalker");
	ADD(CMilitaryOutfit			,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_MILITARY	,"equ_military");
	ADD(CExoOutfit				,CSE_ALifeItemCustomOutfit		,CLSID_EQUIPMENT_EXO		,"equ_exo");

	// Grenades
	ADD(CF1						,CSE_ALifeItemGrenade			,CLSID_GRENADE_F1			,"wpn_grenade_f1");
	ADD(CRGD5					,CSE_ALifeItemGrenade			,CLSID_GRENADE_RGD5			,"wpn_grenade_rgd5");

	// Rockets
	ADD(CExplosiveRocket		,CSE_Temporary					,CLSID_OBJECT_G_RPG7		,"wpn_grenade_rpg7");
	ADD(CExplosiveRocket		,CSE_Temporary					,CLSID_OBJECT_G_FAKE		,"wpn_grenade_fake");

	// Zones
	ADD(CCustomZone				,CSE_ALifeAnomalousZone			,CLSID_ZONE					,"zone");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_MBALD				,"zone_mosquito_bald");
	ADD(CMincer					,CSE_ALifeAnomalousZone			,CLSID_Z_MINCER				,"zone_mincer");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_ACIDF				,"zone_acid_fog");
	ADD(CGraviZone				,CSE_ALifeAnomalousZone			,CLSID_Z_GALANT				,"zone_galantine");
	ADD(CRadioactiveZone		,CSE_ALifeAnomalousZone			,CLSID_Z_RADIO				,"zone_radioactive");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_BFUZZ				,"zone_bfuzz");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_RUSTYH				,"zone_rusty_hair");
	ADD(CMosquitoBald			,CSE_ALifeAnomalousZone			,CLSID_Z_DEAD				,"zone_dead");
	ADD(CLevelChanger			,CSE_ALifeLevelChanger			,CLSID_LEVEL_CHANGER		,"level_changer");
	ADD(CScriptZone				,CSE_ALifeSpaceRestrictor		,CLSID_SCRIPT_ZONE			,"script_zone");
	ADD(CTeamBaseZone			,CSE_ALifeTeamBaseZone			,CLSID_Z_TEAM_BASE			,"team_base_zone");
	ADD(CTorridZone				,CSE_ALifeTorridZone			,CLSID_Z_TORRID				,"torrid_zone");
	ADD(CSpaceRestrictor		,CSE_ALifeSpaceRestrictor		,CLSID_SPACE_RESTRICTOR		,"space_restrictor");

	// Detectors
	ADD(CSimpleDetector			,CSE_ALifeItemDetector			,CLSID_DETECTOR_SIMPLE		,"device_detector_simple");

	// Devices
	ADD(CTorch					,CSE_ALifeItemTorch				,CLSID_DEVICE_TORCH			,"device_torch");
	ADD(CPda					,CSE_ALifeItemPDA				,CLSID_DEVICE_PDA			,"device_pda");
	ADD(CArtefactMerger			,CSE_ALifeItem					,CLSID_DEVICE_AF_MERGER		,"device_art_merger");

	// objects
	ADD(CProjector				,CSE_ALifeObjectProjector		,CLSID_OBJECT_PROJECTOR		,"projector");
	ADD(CWeaponMounted			,CSE_ALifeMountedWeapon			,CLSID_OBJECT_W_MOUNTED		,"wpn_mounted");

	// entity
	ADD(CHangingLamp			,CSE_ALifeObjectHangingLamp		,CLSID_OBJECT_HLAMP			,"hanging_lamp");
	ADD(CPhysicObject			,CSE_ALifeObjectPhysic			,CLSID_OBJECT_PHYSIC		,"obj_physic");
	ADD(CScriptMonster			,CSE_ALifeDynamicObjectVisual	,CLSID_SCRIPT_OBJECT		,"script_object");
	ADD(CBreakableObject		,CSE_ALifeObjectBreakable		,CLSID_OBJECT_BREAKABLE		,"obj_breakable");
	ADD(CPhysicsSkeletonObject	,CSE_ALifePHSkeletonObject		,CLSID_PH_SKELETON_OBJECT	,"obj_phskeleton");

}