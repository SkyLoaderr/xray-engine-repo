#include "stdafx.h"
#include "game_cl_mp.h"
#include "xr_level_controller.h"
#include "xrMessages.h"
#include "GameObject.h"
#include "Actor.h"
#include "level.h"
#include "hudmanager.h"
#include "ui/UIChatWnd.h"
#include "ui/UIChatLog.h"

game_cl_mp::game_cl_mp()
{
	pChatWnd		= NULL;
	pChatLog		= NULL;
};

game_cl_mp::~game_cl_mp()
{
	xr_delete(pChatWnd);
	xr_delete(pChatLog);
};

CUIGameCustom*		game_cl_mp::createGameUI			()
{
	pChatLog = xr_new<CUIChatLog>();
	pChatLog->Init();
	pChatWnd = xr_new<CUIChatWnd>(pChatLog);
	pChatWnd->Init();
	pChatWnd->SetOwner(this);
	return NULL;
};

bool game_cl_mp::CanBeReady	()
{
	return true;
}

bool	game_cl_mp::OnKeyboardPress			(int key)
{
	if ( kJUMP == key || kWPN_FIRE == key )
	{
		bool b_need_to_send_ready = false;

		CObject* curr = Level().CurrentEntity();
		if (!curr) return(false);

		bool is_actor		= curr->SUB_CLS_ID==CLSID_OBJECT_ACTOR;
		bool is_spectator	= curr->SUB_CLS_ID==CLSID_SPECTATOR;
		
		game_PlayerState* ps	= local_player;

		if (is_actor){
			b_need_to_send_ready = ((GAME_PHASE_PENDING == Phase() ) || 
									true == ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) ) && 
									(kWPN_FIRE == key);
		};
		if(is_spectator){
			b_need_to_send_ready =	( GAME_PHASE_PENDING	== Phase() && kWPN_FIRE == key) || 
									( kJUMP == key && GAME_PHASE_INPROGRESS	== Phase() && CanBeReady() );

		};
		if(b_need_to_send_ready){
				CGameObject* GO = smart_cast<CGameObject*>(curr);
				NET_Packet			P;
				GO->u_EventGen		(P,GE_GAME_EVENT,GO->ID()	);
				P.w_u16(GAME_EVENT_PLAYER_READY);
				GO->u_EventSend			(P);
				return true;
		}
		else
			return false;
	};

/*from actorinput.cpp
	if ( (GAME_PHASE_PENDING == Game().phase) || !g_Alive() )
	{
		if (kWPN_FIRE == cmd)	
		{
			// Switch our "ready" status
			NET_Packet			P;
//			u_EventGen			(P,GEG_PLAYER_READY,ID());
			u_EventGen		(P,GE_GAME_EVENT,ID()	);
			P.w_u16(GAME_EVENT_PLAYER_READY);

			u_EventSend			(P);
		}
		return;
	}

*/
/*from spectator.cpp
	if (kJUMP == cmd || kWPN_FIRE == cmd)
	{
		if ((GAME_PHASE_PENDING	== Game().phase && kWPN_FIRE == cmd) || 
			(kJUMP == cmd && GAME_PHASE_INPROGRESS	== Game().phase && HUD().GetUI()->UIGame()->CanBeReady()))
		{
			NET_Packet			P;

			u_EventGen		(P,GE_GAME_EVENT,ID()	);
			P.w_u16(GAME_EVENT_PLAYER_READY);

			u_EventSend			(P);
			return;
		}
		else
			return;
	};
*/

	if( (Phase() != GAME_PHASE_INPROGRESS) && (kQUIT != key) && (kCONSOLE != key))
		return true;

	if (Phase() == GAME_PHASE_INPROGRESS)
	{
		if ((kCHAT == key || kCHAT_TEAM == key) && pChatWnd)
		{
			ref_str prefix;

			if (kCHAT_TEAM == key)
			{
				prefix = "to team> ";
				pChatWnd->TeamChat();
			}
			else
			{
				prefix = "to all> ";
				pChatWnd->AllChat();
			}

			pChatWnd->SetEditBoxPrefix(prefix);

			StartStopMenu(pChatWnd, false);
			if (!pChatWnd->IsShown() && xr_strlen(pChatWnd->UIEditBox.GetText()) > 0)
			{
				ref_str phrase = pChatWnd->UIEditBox.GetText();
				pChatWnd->Say(phrase);
				kCHAT == key ? ChatSayAll(phrase) : ChatSayTeam(phrase);
				pChatWnd->UIEditBox.SetText("");
			}
			return false;
		}
	}

	return inherited::OnKeyboardPress(key);
}

bool	game_cl_mp::OnKeyboardRelease		(int key)
{
	return inherited::OnKeyboardRelease(key);
}

void game_cl_mp::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
	char	Color_Weapon[]	= "%c255,0,0";
	LPSTR	Color_Teams[3]	= {"%c255,255,255", "%c64,255,64", "%c64,64,255"};
	char	Color_Main[]	= "%c192,192,192";

	switch(msg)	{
	
	case GAME_EVENT_PLAYER_KILLED: //dm
		{
			u16 PlayerID, KillerID, WeaponID;
			P.r_u16 (PlayerID);
			P.r_u16 (KillerID);
			P.r_u16 (WeaponID);

			game_PlayerState* pPlayer = GetPlayerByGameID(PlayerID);
			game_PlayerState* pKiller = GetPlayerByGameID(KillerID);
			CObject* pWeapon = Level().Objects.net_Find(WeaponID);
			if (!pPlayer || !pKiller) break;
			
			if (pKiller && pWeapon && WeaponID != 0)
				sprintf(Text, "%s%s %skilled from %s%s %sby %s%s", 
								Color_Teams[pPlayer->team], 
								pPlayer->name, 
								Color_Main,
								Color_Weapon,
								*(pWeapon->cName()), 
								Color_Main,
								Color_Teams[pKiller->team], 
								pKiller->name);
			else
				sprintf(Text, "%s%s %skilled by %s%s",
								Color_Teams[pPlayer->team],
								pPlayer->name, 
								Color_Main,
								Color_Teams[pKiller->team],
								pKiller->name);

			CommonMessageOut(Text);
		}break;
	default:
		inherited::TranslateGameMessage(msg,P);
	}
}


//////////////////////////////////////////////////////////////////////////

void game_cl_mp::ChatSayAll(const ref_str &phrase)
{
	int x=0;
	x=x;
}

//////////////////////////////////////////////////////////////////////////

void game_cl_mp::ChatSayTeam(const ref_str &phrase)
{
	int x=0;
	x=x;
}

//////////////////////////////////////////////////////////////////////////

void game_cl_mp::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	static offFlag = false;
	if (!offFlag)
	{
		if (HUD().GetUI()->UIGame())
		{
			HUD().GetUI()->UIGame()->AddDialogToRender(pChatLog);
			offFlag = true;
		}
	}
}