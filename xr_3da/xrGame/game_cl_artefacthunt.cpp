#include "stdafx.h"
#include "game_cl_artefacthunt.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"
#include "UIGameAHunt.h"
#include "clsid_game.h"
#include "map_manager.h"
#include "LevelGameDef.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "actor.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UISkinSelector.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIMapDesc.h"
#include "ui/UIMessageBoxEx.h"
#include "ui/UIProgressShape.h"
#include "xr_level_controller.h"
#include "Artifact.h"
#include "map_location.h"
#include "Inventory.h"
#include "ActorCondition.h"
#include "ui/TeamInfo.h"

#define TEAM0_MENU		"artefacthunt_team0"
#define	TEAM1_MENU		"artefacthunt_team1"
#define	TEAM2_MENU		"artefacthunt_team2"

#define MESSAGE_MENUS	"ahunt_messages_menu"

#include "game_cl_artefacthunt_snd_msg.h"

game_cl_ArtefactHunt::game_cl_ArtefactHunt()
{
	m_game_ui = NULL;
	/*
	pMessageSounds[0].create(TRUE, "messages\\multiplayer\\mp_artifact_lost");
	pMessageSounds[1].create(TRUE, "messages\\multiplayer\\mp_artifact_on_base");
	pMessageSounds[2].create(TRUE, "messages\\multiplayer\\mp_artifact_on_base_radio");
	pMessageSounds[3].create(TRUE, "messages\\multiplayer\\mp_got_artifact");
	pMessageSounds[4].create(TRUE, "messages\\multiplayer\\mp_got_artifact_radio");
	pMessageSounds[5].create(TRUE, "messages\\multiplayer\\mp_new_artifact");
	pMessageSounds[6].create(TRUE, "messages\\multiplayer\\mp_artifact_delivered_by_enemy");
	pMessageSounds[7].create(TRUE, "messages\\multiplayer\\mp_artifact_stolen");
	*/
	
	m_bBuyEnabled	= FALSE;
	//---------------------------------
	m_Eff_Af_Spawn = "";
	m_Eff_Af_Disappear = "";
	//---------------------------------
	LoadSndMessages();
}

void game_cl_ArtefactHunt::Init ()
{
	LoadTeamData(TEAM1_MENU);
	LoadTeamData(TEAM2_MENU);

	old_artefactBearerID = 0;
	old_artefactID = 0;
	old_teamInPossession = 0;
	//---------------------------------------------------
	string256	fn_game;
	if (FS.exist(fn_game, "$level$", "level.game")) 
	{
		IReader *F = FS.r_open	(fn_game);
		IReader *O = 0;

		// Load RPoints
		if (0!=(O = F->open_chunk	(RPOINT_CHUNK)))
		{ 
			for (int id=0; O->find_chunk(id); ++id)
			{
				RPoint					R;
				u8						RP_team;
				u8						RP_type;
				u8						RP_GameType;

				O->r_fvector3			(R.P);
				O->r_fvector3			(R.A);
				RP_team					= O->r_u8	();	VERIFY(RP_team>=0 && RP_team<4);
				RP_type					= O->r_u8	();
				RP_GameType				= O->r_u8	();
				//u16 res					= 
				O->r_u8	();

				if (RP_GameType != rpgtGameAny && RP_GameType != rpgtGameArtefactHunt)
				{
					continue;					
				};
				switch (RP_type)
				{
				case rptTeamBaseParticle:
					{
						string256 ParticleStr;
						sprintf(ParticleStr, "teambase_particle_%d", RP_team);
						if (pSettings->line_exist("artefacthunt_gamedata", ParticleStr))
						{
							Fmatrix			transform;
							transform.identity();
							transform.setXYZ(R.A);
							transform.translate_over(R.P);
							CParticlesObject* pStaticParticles			= CParticlesObject::Create(pSettings->r_string("artefacthunt_gamedata", ParticleStr),FALSE);
							pStaticParticles->UpdateParent	(transform,zero_vel);
							pStaticParticles->Play			();
							Level().m_StaticParticles.push_back		(pStaticParticles);
						};
					}break;
				};
			};
			O->close();
		}

		FS.r_close	(F);
	}
	//-------------------------------------------------------
	if (pSettings->line_exist("artefacthunt_gamedata", "artefact_spawn_effect"))
		m_Eff_Af_Spawn = pSettings->r_string("artefacthunt_gamedata", "artefact_spawn_effect");
	if (pSettings->line_exist("artefacthunt_gamedata", "artefact_disappear_effect"))
		m_Eff_Af_Disappear = pSettings->r_string("artefacthunt_gamedata", "artefact_disappear_effect");
};

game_cl_ArtefactHunt::~game_cl_ArtefactHunt()
{
	/*
	pMessageSounds[0].destroy();
	pMessageSounds[1].destroy();
	pMessageSounds[2].destroy();
	pMessageSounds[3].destroy();
	pMessageSounds[4].destroy();
	pMessageSounds[5].destroy();
	pMessageSounds[6].destroy();
	pMessageSounds[7].destroy();
	*/
}


BOOL	bBearerCantSprint = TRUE;
void game_cl_ArtefactHunt::net_import_state	(NET_Packet& P)
{
	inherited::net_import_state	(P);
	
	P.r_u8	(artefactsNum);
	P.r_u16	(artefactBearerID);
	P.r_u8	(teamInPossession);
	P.r_u16	(artefactID);
	bBearerCantSprint = !!P.r_u8();

	P.r_u32(iReinforcementTime);
	if (iReinforcementTime != 0)
	{
		P.r_s32	(dReinforcementTime);
		dReinforcementTime += Level().timeServer();
	}
	else
		dReinforcementTime = 0;
}


void game_cl_ArtefactHunt::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
//	LPSTR	Color_Teams[3]		= {"%c<255,255,255,255>", "%c<255,64,255,64>", "%c<255,64,64,255>"};
	char	Color_Main[]		= "%c<255,192,192,192>";
	char	Color_Artefact[]	= "%c<255,255,255,0>";
//	LPSTR	TeamsNames[3]		= {"Zero Team", "Team Green", "Team Blue"};

	switch(msg)	{
//-------------------UI MESSAGES
	case GAME_EVENT_ARTEFACT_TAKEN: //ahunt
		{
			u16 PlayerID, Team;
			P.r_u16 (PlayerID);
			P.r_u16 (Team);

			game_PlayerState* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %shas taken the %sArtefact", 
				CTeamInfo::GetTeam_color_tag(int(Team)), 
				pPlayer->name, 
				Color_Main,
				Color_Artefact);
			CommonMessageOut(Text);

			if (!Game().local_player) break;
			if (Game().local_player->GameID == PlayerID)
				PlaySndMessage(ID_GOT_AF);
//				pMessageSounds[3].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
			else
				if (Game().local_player->team == Team)
					PlaySndMessage(ID_GOT_AF_R);
//					pMessageSounds[4].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
				else
					PlaySndMessage(ID_AF_STOLEN);
//					pMessageSounds[7].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
		}break;
	case GAME_EVENT_ARTEFACT_DROPPED: //ahunt
		{
			u16 PlayerID, Team;
			P.r_u16 (PlayerID);
			P.r_u16 (Team);

			game_PlayerState* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %shas dropped the %sArtefact", 
				CTeamInfo::GetTeam_color_tag(int(Team)), 
				pPlayer->name, 
				Color_Main,
				Color_Artefact);
			CommonMessageOut(Text);

//			pMessageSounds[0].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
			PlaySndMessage(ID_AF_LOST);
		}break;
	case GAME_EVENT_ARTEFACT_ONBASE: //ahunt
		{
			u16 PlayerID, Team;
			P.r_u16 (PlayerID);
			P.r_u16 (Team);

			game_PlayerState* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %sscores", 
				CTeamInfo::GetTeam_color_tag(int(Team)), 
				CTeamInfo::GetTeam_name(int(Team)),
				Color_Main);
			CommonMessageOut(Text);
			
			if (!Game().local_player) break;
			if (Game().local_player->GameID == PlayerID)
//				pMessageSounds[1].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
				PlaySndMessage(ID_AF_ONBASE);
			else
				if (Game().local_player->team == Team)
//					pMessageSounds[2].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
					PlaySndMessage(ID_AF_ONBASE_R);
				else
//					pMessageSounds[6].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
					PlaySndMessage(ID_AF_ENEMY);
		}break;
	case GAME_EVENT_ARTEFACT_SPAWNED: //ahunt
		{
			sprintf(Text, "%sArtefact has been spawned. Bring it to your base to score.", 
				Color_Main);
			CommonMessageOut(Text);

			PlaySndMessage(ID_NEW_AF);
//			pMessageSounds[5].play_at_pos(NULL, Fvector().set(0,0,0), sm_2D, 0);
		}break;
	case GAME_EVENT_ARTEFACT_DESTROYED:  //ahunt
		{
			sprintf(Text, "%sArtefact has been destroyed.", 
				Color_Main);
			u16 ArtefactID = P.r_u16();
			//-------------------------------------------
			CObject* pObj = Level().Objects.net_Find(ArtefactID);
			if (pObj && xr_strlen(m_Eff_Af_Disappear))
				PlayParticleEffect(m_Eff_Af_Disappear.c_str(), pObj->Position());
			//-------------------------------------------
			CommonMessageOut(Text);
		}break;
	default:
		inherited::TranslateGameMessage(msg,P);
	}
}

CUIGameCustom* game_cl_ArtefactHunt::createGameUI()
{
	game_cl_mp::createGameUI();

	CLASS_ID clsid			= CLSID_GAME_UI_ARTEFACTHUNT;
	m_game_ui	= smart_cast<CUIGameAHunt*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(m_game_ui);
	m_game_ui->SetClGame(this);
	m_game_ui->Init();

	//----------------------------------------------------------------
	pBuyMenuTeam1 = InitBuyMenu("artefacthunt_base_cost", 1);
	LoadTeamDefaultPresetItems(TEAM1_MENU, pBuyMenuTeam1, &PresetItemsTeam1);
	pBuyMenuTeam2 = InitBuyMenu("artefacthunt_base_cost", 2);
	LoadTeamDefaultPresetItems(TEAM2_MENU, pBuyMenuTeam2, &PresetItemsTeam2);
	//----------------------------------------------------------------
	pSkinMenuTeam1 = InitSkinMenu(1);
	pSkinMenuTeam2 = InitSkinMenu(2);

	pInventoryMenu = xr_new<CUIInventoryWnd>();
	//-----------------------------------------------------------	
	pPdaMenu = xr_new<CUIPdaWnd>();
	//-----------------------------------------------------------
	pMapDesc = xr_new<CUIMapDesc>();
	pMapDesc->SetWorkPhase(GAME_PHASE_INPROGRESS);
	//-----------------------------------------------------------
	LoadMessagesMenu(MESSAGE_MENUS);
	//-----------------------------------------------------------
	return m_game_ui;
}

void game_cl_ArtefactHunt::GetMapEntities(xr_vector<SZoneMapEntityData>& dst)
{
	inherited::GetMapEntities(dst);

	SZoneMapEntityData D;
	u32 color_enemy_with_artefact		=		0xffff0000;
	u32 color_artefact					=		0xffffffff;
	u32 color_friend_with_artefact		=		0xffffff00;


	s16 local_team						=		local_player->team;


	CObject* pObject = Level().Objects.net_Find(artefactID);
	if(!pObject)
		return;

	CArtefact* pArtefact = smart_cast<CArtefact*>(pObject);
	VERIFY(pArtefact);

	CObject* pParent = pArtefact->H_Parent();
	if(!pParent){// Artefact alone
		D.color	= color_artefact;
		D.pos	= pArtefact->Position();
		dst.push_back(D);
		return;
	};

	if (pParent && pParent->ID() == artefactBearerID && GetPlayerByGameID(artefactBearerID)){
		CObject* pBearer = Level().Objects.net_Find(artefactBearerID);
		VERIFY(pBearer);
		D.pos	= pBearer->Position();

		game_PlayerState*	ps  =	GetPlayerByGameID		(artefactBearerID);
		(ps->team==local_team)? D.color=color_friend_with_artefact:D.color=color_enemy_with_artefact;
		
		//remove previous record about this actor !!!
		dst.push_back(D);
		return;
	}

}

void game_cl_ArtefactHunt::shedule_Update			(u32 dt)
{
	inherited::shedule_Update		(dt);
	
	//out game information
//	m_game_ui->SetReinforcementCaption("");
	m_game_ui->SetBuyMsgCaption		("");
	m_game_ui->SetTodoCaption		("");
	m_game_ui->SetPressBuyMsgCaption	("");	

	switch (phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			if (local_player)
			{
				if (local_player->testFlag(GAME_PLAYER_FLAG_ONBASE))
				{
					m_bBuyEnabled = TRUE;
				}
				else
				{
					m_bBuyEnabled = FALSE;
				};
			};			

			if (local_player && Level().CurrentControlEntity())
			{
				if (Level().CurrentControlEntity()->CLS_ID == CLSID_OBJECT_ACTOR)
				{
					if(m_game_ui) m_game_ui->SetBuyMsgCaption("");
					if (m_bBuyEnabled)
					{
						if (!(pCurBuyMenu && pCurBuyMenu->IsShown()) && 
							!(pCurSkinMenu && pCurSkinMenu->IsShown()))
						{					
							if(m_game_ui) m_game_ui->SetBuyMsgCaption("Press B to access Buy Menu");
						};
					}					
					
					if (m_game_ui)
					{
						if (local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
							m_game_ui->SetPressJumpMsgCaption("Press Fire to become a Spectator");
						else
							m_game_ui->SetPressJumpMsgCaption("");
					};
				}
				else
				{					
					if(m_game_ui) m_game_ui->SetBuyMsgCaption("");
					if (m_bTeamSelected && m_bSkinSelected)
					{
						if (iReinforcementTime != 0)
						{
							if (!pBuySpawnMsgBox->IsShown() && 							
								(local_player->money_for_round+m_iSpawn_Cost)>=0)
							{
								if (m_game_ui) m_game_ui->SetPressJumpMsgCaption("Press Jump to pay for immediate spawn");
							}
							else
							{
								if (m_game_ui) m_game_ui->SetPressJumpMsgCaption("");
							}
						}
						else
						{
							if (m_game_ui) m_game_ui->SetPressJumpMsgCaption("Press Jump to spawn immediately");
						};
					}
					else
					{
						if (!m_bTeamSelected)
							if (m_game_ui) m_game_ui->SetPressJumpMsgCaption("Press Jump to select team");
							else
								if (!m_bSkinSelected)
									if (m_game_ui) m_game_ui->SetPressJumpMsgCaption("Press Jump to select skin");
					}
				};				
			}

			if (local_player/*pCurActor && !m_game_ui->pCurBuyMenu->IsShown()*/)
			{
				game_TeamState team0 = teams[0];
				game_TeamState team1 = teams[1];

				if (dReinforcementTime != 0 && Level().CurrentViewEntity() && m_cl_dwWarmUp_Time == 0)
				{
					u32 CurTime = Level().timeServer();
					u32 dTime;
					if (s32(CurTime) > dReinforcementTime) dTime = 0;
					else dTime = iCeil(float(dReinforcementTime - CurTime) / 1000);
							
					m_game_ui->m_pReinforcementInidcator->SetPos(dTime, iReinforcementTime/1000);
				}else
					m_game_ui->m_pReinforcementInidcator->SetPos(0, 0);


				s16 lt = local_player->team;
				if (lt>=0)
				{
					if(m_game_ui)
						m_game_ui->SetScoreCaption	(teams[0].score, teams[1].score);
				};				
	/*
			if ( (artefactBearerID==0))// && (artefactID!=0) )
				{
					m_game_ui->SetTodoCaption("Grab the Artefact");
				}
				else
				{
					if (teamInPossession != local_player->team )
					{
						m_game_ui->SetTodoCaption("Stop ArtefactBearer");
					}
					else
					{
						if (local_player->GameID == artefactBearerID)
						{
							m_game_ui->SetTodoCaption("You got the Artefact. Bring it to your base.");
						}
						else
						{
							m_game_ui->SetTodoCaption("Protect your ArtefactBearer");
						};
					};
				};
			*/
			};

			if (Level().CurrentViewEntity() && m_game_ui)
			{
				game_PlayerState* ps = GetPlayerByGameID(Level().CurrentViewEntity()->ID());
				if (ps&&m_game_ui) m_game_ui->SetRank(ps->team, ps->rank);
				if (ps&&m_game_ui) m_game_ui->SetFraglimit(ps->kills, artefactsNum);
			}
		}break;
	case GAME_PHASE_TEAM1_ELIMINATED:
		{
//			HUD().GetUI()->HideIndicators();
//			GetUICursor()->Hide();
			m_game_ui->SetRoundResultCaption("Team Green ELIMINATED!");
		}break;
	case GAME_PHASE_TEAM2_ELIMINATED:
		{
//			HUD().GetUI()->HideIndicators();
//			GetUICursor()->Hide();
			m_game_ui->SetRoundResultCaption("Team Blue ELIMINATED!");
		}break;
	default:
		{
			
		}break;
	};

}

BOOL game_cl_ArtefactHunt::CanCallBuyMenu			()
{
	if (!m_bBuyEnabled) return FALSE;
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;
	
	if (pUITeamSelectWnd && pUITeamSelectWnd->IsShown())
	{
		return FALSE;
	};
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
	{
		return FALSE;
	};
	if (pInventoryMenu && pInventoryMenu->IsShown())
	{
		return FALSE;
	};

	CActor* pCurActor = smart_cast<CActor*> (Level().CurrentEntity());
	if (!pCurActor || !pCurActor->g_Alive()) return FALSE;

	return TRUE;
};

bool game_cl_ArtefactHunt::CanBeReady				()
{
	if (!local_player) return false;
	m_bMenuCalledFromReady = TRUE;

	SetCurrentSkinMenu();
	SetCurrentBuyMenu();

	if (!m_bTeamSelected)
	{
		if (CanCallTeamSelectMenu())
			StartStopMenu(pUITeamSelectWnd,true);
		return false;
	};
/*
	if (!m_bSkinSelected)
	{
		if (CanCallSkinMenu())
			StartStopMenu(pCurSkinMenu,true);
		return false;
	};
*/
	if (pCurBuyMenu && !pCurBuyMenu->IsShown())
		ClearBuyMenu();

	m_bMenuCalledFromReady = FALSE;
	return true;
};

char*	game_cl_ArtefactHunt::getTeamSection(int Team)
{
	switch (Team)
	{
	case 1:
		{
			return "artefacthunt_team1";
		}break;
	case 2:
		{
			return "artefacthunt_team2";
		}break;
	default:
		NODEFAULT;
	};
#ifdef DEBUG
	return NULL;
#endif
};

bool	game_cl_ArtefactHunt::PlayerCanSprint			(CActor* pActor)
{
	if (artefactBearerID == 0) return true;
	if (bBearerCantSprint && pActor->ID() == artefactBearerID) return false;
	return true;
};

#define ARTEFACT_NEUTRAL "mp_af_neutral_location"
#define ARTEFACT_FRIEND "mp_af_friend_location"
#define ARTEFACT_ENEMY "mp_af_enemy_location"

void	game_cl_ArtefactHunt::UpdateMapLocations		()
{
	inherited::UpdateMapLocations();

	if (local_player)
	{
		if (!artefactID)
		{
			if (old_artefactID)
				Level().MapManager().RemoveMapLocationByObjectID(old_artefactID);
		}
		else
		{
			if (!artefactBearerID)
			{
				if (!Level().MapManager().HasMapLocation(ARTEFACT_NEUTRAL, artefactID))
				{
					Level().MapManager().RemoveMapLocationByObjectID(artefactID);
					(Level().MapManager().AddMapLocation(ARTEFACT_NEUTRAL, artefactID))->EnablePointer();
				};
			}
			else
			{
				if (teamInPossession == local_player->team)
				{
					if (!Level().MapManager().HasMapLocation(ARTEFACT_FRIEND, artefactID))
					{
						Level().MapManager().RemoveMapLocationByObjectID(artefactID);
						(Level().MapManager().AddMapLocation(ARTEFACT_FRIEND, artefactID))->EnablePointer();
					}
				}
				else
				{
					if (!Level().MapManager().HasMapLocation(ARTEFACT_ENEMY, artefactID))
					{
						Level().MapManager().RemoveMapLocationByObjectID(artefactID);
					};

					bool OutfitWorkDown = false;

					CActor* pActor = smart_cast<CActor*>(Level().Objects.net_Find(artefactBearerID));
					if (pActor)
					{
						CCustomOutfit* pOutfit			= (CCustomOutfit*)pActor->inventory().m_slots[OUTFIT_SLOT].m_pIItem;
						if (pOutfit && pOutfit->CLS_ID == CLSID_EQUIPMENT_SCIENTIFIC)
						{
							if (!pActor->AnyAction())
							{
								if (Level().MapManager().HasMapLocation(ARTEFACT_ENEMY, artefactID))
								{
									Level().MapManager().RemoveMapLocationByObjectID(artefactID);									
								}
								OutfitWorkDown = true;
							}
						}
					}
					if (!OutfitWorkDown && !Level().MapManager().HasMapLocation(ARTEFACT_ENEMY, artefactID))
					{
						(Level().MapManager().AddMapLocation(ARTEFACT_ENEMY, artefactID))->EnablePointer();
					}
				}
			};
		};
		old_artefactBearerID = artefactBearerID;
		old_artefactID = artefactID;
		old_teamInPossession = teamInPossession;
	};
};

bool	game_cl_ArtefactHunt::NeedToSendReady_Spectator			(int key, game_PlayerState* ps)
{
	bool res = ( GAME_PHASE_PENDING	== Phase() && kWPN_FIRE == key) || 
		( (/*kWPN_FIRE == key || */kJUMP == key) && GAME_PHASE_INPROGRESS	== Phase() && 
		CanBeReady());
	
	if (GAME_PHASE_INPROGRESS == Phase() && kJUMP == key &&
		iReinforcementTime != 0 && 
		!pBuySpawnMsgBox->IsShown() && 
		local_player && (local_player->money_for_round+m_iSpawn_Cost)>=0) 
	{
		string1024	BuySpawnText;
		sprintf(BuySpawnText, "You have %d$. You can buy a spawn for %d$. Press Yes to pay.", 
			abs(local_player->money_for_round), abs(m_iSpawn_Cost));
		pBuySpawnMsgBox->SetText(BuySpawnText);

		if (m_bTeamSelected && m_bSkinSelected)
			StartStopMenu(pBuySpawnMsgBox, true);
		return false;
	};
	return res;
}

void	game_cl_ArtefactHunt::OnSpawn					(CObject* pObj)
{
	inherited::OnSpawn(pObj);
	if (!pObj) return;
	CArtefact* pArtefact = smart_cast<CArtefact*>(pObj);
	if (pArtefact)
	{
		if (xr_strlen(m_Eff_Af_Spawn))
			PlayParticleEffect(m_Eff_Af_Spawn.c_str(), pObj->Position());
	};	
}

void	game_cl_ArtefactHunt::OnDestroy				(CObject* pObj)
{	
	inherited::OnDestroy(pObj);
	if (!pObj) return;
};

void	game_cl_ArtefactHunt::LoadSndMessages				()
{
	LoadSndMessage("ahunt_snd_messages", "artifact_lost", ID_AF_LOST);
	LoadSndMessage("ahunt_snd_messages", "artifact_on_base", ID_AF_ONBASE);
	LoadSndMessage("ahunt_snd_messages", "artifact_on_base_radio", ID_AF_ONBASE_R);
	LoadSndMessage("ahunt_snd_messages", "got_artifact", ID_GOT_AF);
	LoadSndMessage("ahunt_snd_messages", "got_artifact_radio", ID_GOT_AF_R);
	LoadSndMessage("ahunt_snd_messages", "new_artifact", ID_NEW_AF);
	LoadSndMessage("ahunt_snd_messages", "artifact_delivered_by_enemy", ID_AF_ENEMY);
	LoadSndMessage("ahunt_snd_messages", "artifact_stolen", ID_AF_STOLEN);
};

void	game_cl_ArtefactHunt::OnBuySpawnMenu_Ok		()
{
	CObject* curr = Level().CurrentEntity();
	if (!curr) return;
	CGameObject* GO = smart_cast<CGameObject*>(curr);
	NET_Packet			P;
	GO->u_EventGen		(P,GE_GAME_EVENT,GO->ID()	);
	P.w_u16(GAME_EVENT_PLAYER_BUY_SPAWN);
	GO->u_EventSend			(P);
};

void	game_cl_ArtefactHunt::OnSellItemsFromRuck		()
{
	if (!local_player || local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) || !local_player->testFlag(GAME_PLAYER_FLAG_ONBASE)) return;
	CActor* pCurActor = smart_cast<CActor*> (Level().Objects.net_Find	(local_player->GameID));
	if (!pCurActor) return;
	
	TIItemContainer::const_iterator	IRuck = pCurActor->inventory().m_ruck.begin();
	TIItemContainer::const_iterator	ERuck = pCurActor->inventory().m_ruck.end();

	NET_Packet	P;
	pCurActor->u_EventGen(P, GEG_PLAYER_ITEM_SELL, pCurActor->ID());
	P.w_u16(u16(pCurActor->inventory().m_ruck.size() & 0xffff));
	for ( ; IRuck != ERuck; ++IRuck) 
	{		
		PIItem pItem = *IRuck;
		P.w_u16		(pItem->object().ID());		
	};	
	pCurActor->u_EventSend(P);
};
/*
extern BOOL	g_bShildedBases;
void			game_cl_ArtefactHunt::OnPlayerFlagsChanged	(game_PlayerState* ps)
{
	inherited::OnPlayerFlagsChanged(ps);
	if (!ps) return;
	if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) return;
	CObject* pObject = Level().Objects.net_Find(ps->GameID);
	if (!pObject) return;

	if (pObject->CLS_ID != CLSID_OBJECT_ACTOR) return;
	CActor* pActor = smart_cast<CActor*>(pObject);
	if (!pActor) return;
	pActor->conditions().SetCanBeHarmedState(!g_bShildedBases || !ps->testFlag(GAME_PLAYER_FLAG_ONBASE));
};
*/