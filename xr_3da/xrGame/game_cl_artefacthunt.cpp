#include "stdafx.h"
#include "game_cl_artefacthunt.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "level.h"
#include "UIGameAHunt.h"

game_cl_ArtefactHunt::game_cl_ArtefactHunt()
{

	pMessageSounds[0].create(TRUE, "messages\\multiplayer\\mp_artifact_lost");
	pMessageSounds[1].create(TRUE, "messages\\multiplayer\\mp_artifact_on_base");
	pMessageSounds[2].create(TRUE, "messages\\multiplayer\\mp_artifact_on_base_radio");
	pMessageSounds[3].create(TRUE, "messages\\multiplayer\\mp_got_artifact");
	pMessageSounds[4].create(TRUE, "messages\\multiplayer\\mp_got_artifact_radio");
	pMessageSounds[5].create(TRUE, "messages\\multiplayer\\mp_new_artifact");

	pMessageSounds[6].create(TRUE, "messages\\multiplayer\\mp_artifact_delivered_by_enemy");
	pMessageSounds[7].create(TRUE, "messages\\multiplayer\\mp_artifact_stolen.ogg ");
}

game_cl_ArtefactHunt::~game_cl_ArtefactHunt()
{
	pMessageSounds[0].destroy();
	pMessageSounds[1].destroy();
	pMessageSounds[2].destroy();
	pMessageSounds[3].destroy();
	pMessageSounds[4].destroy();
	pMessageSounds[5].destroy();
	pMessageSounds[6].destroy();
	pMessageSounds[7].destroy();
}


void game_cl_ArtefactHunt::net_import_state	(NET_Packet& P)
{
	inherited::net_import_state	(P);
	
	P.r_u8	(artefactsNum);
	P.r_u16	(artefactBearerID);
	P.r_u8	(teamInPossession);
	P.r_u16	(artefactID);
}


void game_cl_ArtefactHunt::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
	LPSTR	Color_Teams[3]		= {"%c255,255,255", "%c64,255,64", "%c64,64,255"};
	char	Color_Main[]		= "%c192,192,192";
	char	Color_Artefact[]	= "%c255,255,0";
	LPSTR	TeamsNames[3]		= {"Zero Team", "Team Green", "Team Blue"};

	switch(msg)	{
	case GMSG_ARTEFACT_TAKEN: //ahunt
		{
			u16 PlayerID, Team;
			P.r_u16 (PlayerID);
			P.r_u16 (Team);

			game_cl_GameState::Player* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %shas taken the %sArtefact", 
				Color_Teams[Team], 
				pPlayer->name, 
				Color_Main,
				Color_Artefact);
			UIMessageOut(Text);
						
			CActor* pActor = dynamic_cast<CActor*> (Level().CurrentEntity());
			if (!pActor) break;
			if (pActor->ID() == PlayerID)
				pMessageSounds[3].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
			else
				if (pActor->g_Team() == Team)
					pMessageSounds[4].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
				else
					pMessageSounds[7].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
		}break;
	case GMSG_ARTEFACT_DROPPED: //ahunt
		{
			u16 PlayerID, Team;
			P.r_u16 (PlayerID);
			P.r_u16 (Team);

			game_cl_GameState::Player* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %shas dropped the %sArtefact", 
				Color_Teams[Team], 
				pPlayer->name, 
				Color_Main,
				Color_Artefact);
			UIMessageOut(Text);

			pMessageSounds[0].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
		}break;
	case GMSG_ARTEFACT_ONBASE: //ahunt
		{
			u16 PlayerID, Team;
			P.r_u16 (PlayerID);
			P.r_u16 (Team);

			game_cl_GameState::Player* pPlayer = GetPlayerByGameID(PlayerID);
			if (!pPlayer) break;

			sprintf(Text, "%s%s %sscores", 
				Color_Teams[Team], 
				TeamsNames[Team], 
				Color_Main);
			UIMessageOut(Text);

			CActor* pActor = dynamic_cast<CActor*> (Level().CurrentEntity());
			if (!pActor) break;
			if (pActor->ID() == PlayerID)
				pMessageSounds[1].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
			else
				if (pActor->g_Team() == Team)
					pMessageSounds[2].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
				else
					pMessageSounds[6].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
/*
			if (Level().CurrentEntity() && Level().CurrentEntity()->ID() == PlayerID)
				pMessageSounds[1].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
			else
				pMessageSounds[2].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
*/
		}break;
	case GMSG_ARTEFACT_SPAWNED: //ahunt
		{
			sprintf(Text, "%sArtefact has been spawned. Bring it to your base to score.", 
				Color_Main);
			UIMessageOut(Text);

			pMessageSounds[5].play_at_pos(NULL, Device.vCameraPosition, sm_2D, 0);
		}break;
	case GMSG_ARTEFACT_DESTROYED:  //ahunt
		{
			sprintf(Text, "%sArtefact has been destroyed.", 
				Color_Main);
			UIMessageOut(Text);

		}break;
	

	default:
		inherited::TranslateGameMessage(msg,P);
	}
}

CUIGameCustom* game_cl_ArtefactHunt::createGameUI()
{
	CLASS_ID clsid			= CLSID_GAME_UI_ARTEFACTHUNT;
	CUIGameAHunt*			pUIGame	= dynamic_cast<CUIGameAHunt*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(pUIGame);
	pUIGame->SetClGame(this);
	pUIGame->Init();
	return pUIGame;
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

	CArtifact* pArtefact = dynamic_cast<CArtifact*>(pObject);
	VERIFY(pArtefact);

	CObject* pParent = pArtefact->H_Parent();
	if(!pParent){// Artefact alone
		D.color	= color_artefact;
		D.pos	= pArtefact->Position();
		dst.push_back(D);
		return;
	};

	if (pParent && pParent->ID() == artefactBearerID){
		CObject* pBearer = Level().Objects.net_Find(artefactBearerID);
		VERIFY(pBearer);
		D.pos	= pBearer->Position();

		Player*	ps  =	GetPlayerByGameID		(artefactBearerID);
		(ps->team==local_team)? D.color=color_friend_with_artefact:D.color=color_enemy_with_artefact;
		
		//remove previous record about this actor !!!
		dst.push_back(D);
		return;
	}

}
