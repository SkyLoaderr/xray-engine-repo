#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"
#include "ui\UIBuyWeaponWnd.h"
#include "ui\UISkinSelector.h"
#include "HUDManager.h"
#include "level.h"
#include "game_cl_base.h"
#include "Spectator.h"
#include "Inventory.h"
#include "xrserver_objects_alife_items.h"

#include "game_cl_Deathmatch.h"

#define MSGS_OFFS 510

#define	TEAM0_MENU		"deathmatch_team0"
#define TIME_MSG_COLOR		0xffff0000

//--------------------------------------------------------------------
CUIGameDM::CUIGameDM()
{
	m_game			= NULL; 
	ClearLists ();
	
	pBuyMenuTeam0	= NULL;
	pCurBuyMenu		= NULL;
	
	PresetItemsTeam0.clear();
	pCurPresetItems	= NULL;;

	pSkinMenuTeam0	= NULL;
	pCurSkinMenu	= NULL;

	m_bBuyEnabled	= TRUE;

	m_bSkinSelected	= TRUE;

/*
	TimeMsgStatic.SetFont			(HUD().pFontDI);

	TimeMsgDyn.SetFont				(HUD().pFontDI);
	TimeMsgDyn.SetStyleParams		(CUITextBanner::tbsFade)->fPeriod = 0.5f;
	TimeMsgDyn.SetTextColor			(0xffff0000);
*/

	m_time_caption = "timelimit";
	m_gameCaptions.addCustomMessage(m_time_caption, 0.0f, -0.95f, 0.03f, HUD().pFontDI, CGameFont::alCenter, TIME_MSG_COLOR, "");
//	m_gameCaptions.customizeMessage(m_time_caption, CUITextBanner::tbsFlicker)->fPeriod = 0.5f;

	//-----------------------------------------------------------------------
	m_iCurrentPlayersMoney = 0;
}
//--------------------------------------------------------------------
void CUIGameDM::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = dynamic_cast<game_cl_Deathmatch*>(g);
	R_ASSERT(m_game);
}

void	CUIGameDM::Init				()
{

	CUIDMFragList* pFragList		= xr_new<CUIDMFragList>		();
	CUIDMPlayerList* pPlayerList	= xr_new<CUIDMPlayerList>	();

	int ScreenW = Device.dwWidth;
	int ScreenH = Device.dwHeight;
	//-----------------------------------------------------------
	RECT FrameRect = pFragList->GetFrameRect ();
	int FrameW	= FrameRect.right - FrameRect.left;
	int FrameH	= FrameRect.bottom - FrameRect.top;

	pFragList->SetWndRect((ScreenW-FrameW)/2, (ScreenH - FrameH)/2, FrameW, FrameH);

	m_aFragsLists.push_back(pFragList);
	//-----------------------------------------------------------
	FrameRect = pPlayerList->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerList->SetWndRect((ScreenW-FrameW)/2, (ScreenH - FrameH)/2, FrameW, FrameH);
	m_aPlayersLists.push_back(pPlayerList);
	//-----------------------------------------------------------
	string64	Team0;
	std::strcpy(Team0, TEAM0_MENU);
	m_aTeamSections.push_back(Team0);
	//-----------------------------------------------------------
	pBuyMenuTeam0	= InitBuyMenu("deathmatch_base_cost", 0);
	pCurBuyMenu		= pBuyMenuTeam0;
	//-----------------------------------------------------------
	pSkinMenuTeam0	= InitSkinMenu(0);
	pCurSkinMenu	= pSkinMenuTeam0;
};
//--------------------------------------------------------------------

void	CUIGameDM::ClearLists ()
{
	for (u32 i=0; i<m_aFragsLists.size(); ++i)
	{
		xr_delete(m_aFragsLists[i]);
	}
	m_aFragsLists.clear();

	for (u32 i=0; i<m_aPlayersLists.size(); ++i)
	{
		xr_delete(m_aPlayersLists[i]);
	}
	m_aPlayersLists.clear();

	m_aTeamSections.clear();
}
//--------------------------------------------------------------------
CUIGameDM::~CUIGameDM()
{
	ClearLists();

	xr_delete(pBuyMenuTeam0);
}
//--------------------------------------------------------------------

void CUIGameDM::OnFrame()
{
	inherited::OnFrame();

	switch (m_game->phase){
	case GAME_PHASE_PENDING: 
		for (u32 i=0; i<m_aPlayersLists.size(); ++i)
		{
			m_aPlayersLists[i]->Update();
			if (!m_aPlayersLists[i]->IsShown()) 
			{
				m_aPlayersLists[i]->Show();
			};
		};
	break;
	case GAME_PHASE_INPROGRESS:
		{
			if (uFlags&flShowFragList) 
			{
				for (u32 i=0; i<m_aFragsLists.size(); ++i)
				{
					m_aFragsLists[i]->Update();
				};
				//			pFragList->Update();
			};
			if (m_game->timelimit)
			{
				if (Level().timeServer()<(m_game->start_time + m_game->timelimit))
				{
					u32 lts = Level().timeServer();
					u32 Rest = (m_game->start_time + m_game->timelimit) - lts;

					u32 RHour = Rest / 3600000;
					Rest %= 3600000;
					u32 RMinutes = Rest / 60000;
					Rest %= 60000;
					u32 RSecs = Rest / 1000;
					string64 S;
					sprintf(S,"%02d:%02d:%02d", RHour, RMinutes, RSecs);
					SetTimeMsgCaption(S);

//					TimeMsgStatic.Out			(0.f,-0.95f,"%02d:%02d:%02d", RHour, RMinutes, RSecs);
				}
				else
				{
					SetTimeMsgCaption("00:00:00");
//					TimeMsgDyn.Out				(0.f,-0.95f,"00:00:00");
//					TimeMsgDyn.Update			();
				}
			};
			if (!m_bSkinSelected)
			{
				SetCurrentSkinMenu();
				
				if (!pCurSkinMenu->IsShown())
					StartStopMenu(pCurSkinMenu);
			}
			//-----------------------------------------------------------
			game_cl_GameState::Player* P = m_game->local_player;
			if (!P) break;

			string16	tmp;
			_itoa(P->money_for_round, tmp, 10);
			ref_str PMoney(tmp);
			HUD().GetUI()->UIMainIngameWnd.ChangeTotalMoneyIndicator(PMoney);

			if (P->money_for_round != m_iCurrentPlayersMoney)
			{
				s16 dMoney = P->money_for_round - m_iCurrentPlayersMoney;
				if (dMoney > 0)
					sprintf(tmp,"+%d", dMoney);
				else
					sprintf(tmp,"%d", dMoney);				
				
				PMoney._set(tmp);

				HUD().GetUI()->UIMainIngameWnd.DisplayMoneyChange(PMoney);

				m_iCurrentPlayersMoney = P->money_for_round;
			};
			//-----------------------------------------------------------
///			if (Level().CurrentControlEntity() && Level().CurrentControlEntity()->SUB_CLS_ID == CLSID_SPECTATOR)
			if (Level().CurrentEntity() && Level().CurrentEntity()->SUB_CLS_ID == CLSID_SPECTATOR)
			{
				if (!pCurBuyMenu || !pCurBuyMenu->IsShown())
				{
					HUD().pFontDI->SetAligment		(CGameFont::alCenter);

					HUD().pFontDI->SetColor		(0xffffffff);
					HUD().pFontDI->Out			(0.f,0.0f,"SPECTATOR : Free-fly camera");

					float OldSize = HUD().pFontDI->GetSize			();

					HUD().pFontDI->SetSize		(0.02f);
					HUD().pFontDI->SetColor		(0xffffffff);
					HUD().pFontDI->Out			(0.f,0.9f,"Press Jump to start");
					HUD().pFontDI->SetSize		(OldSize);
				};
			};
		}break;
	}

	if (pCurBuyMenu && pCurBuyMenu->IsShown() && !CanCallBuyMenu())
	{
		StartStopMenu(pCurBuyMenu);
	};
}
//--------------------------------------------------------------------

void CUIGameDM::Render()
{
	inherited::Render();

	switch (m_game->phase){
	case GAME_PHASE_PENDING:
		for (u32 i=0; i<m_aPlayersLists.size(); i++)
		{
			m_aPlayersLists[i]->Draw();
		};
//		pPlayerList->Draw();
		break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) 
		{
			//pFragList->Draw	();
			for (u32 i=0; i<m_aFragsLists.size(); i++)
			{
				m_aFragsLists[i]->Draw();
			};
		}break;
	}
}
//--------------------------------------------------------------------

bool CUIGameDM::IR_OnKeyboardPress(int dik)
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;

	switch (dik)
	{
	case DIK_I: 
		StartStopMenu(&InventoryMenu);
		return true;
		break;
	case DIK_ESCAPE:
		if(m_pUserMenu)
		{
			StartStopMenu(m_pUserMenu);
			return true;
		}
		break;
	}

	if (m_game->phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
			case DIK_1:
			case DIK_2:
			case DIK_3:
			case DIK_4:
			case DIK_5:
			case DIK_6:
			case DIK_7:
			case DIK_8:
			case DIK_9:
			case DIK_0:
				{
					if ((pCurBuyMenu && pCurBuyMenu->IsShown()) || 
						(pCurSkinMenu && pCurSkinMenu->IsShown()))
						return true;
				}break;
		case DIK_TAB:	
			{
				SetFlag		(flShowFragList,TRUE);	
				return true;
			}break;
		case DIK_B:
			{
				if (!CanCallBuyMenu()) break;

				SetCurrentBuyMenu	();

				if (pCurBuyMenu && !pCurBuyMenu->IsShown())
					SetBuyMenuItems		();

				StartStopMenu(pCurBuyMenu);
			}break;
		case DIK_N:
			{
				SetCurrentSkinMenu	();

				StartStopMenu(pCurSkinMenu);
			}break;
		}
	}
	return false;
}
//--------------------------------------------------------------------

bool CUIGameDM::IR_OnKeyboardRelease(int dik)
{
	if(inherited::IR_OnKeyboardRelease(dik)) return true;

	if (m_game->phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:
			/*
			pFragList->Hide();
			m_pUserMenu = NULL;			
			*/
			SetFlag		(flShowFragList,FALSE);	
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------
static	u16 SlotsToCheck [] = {
	KNIFE_SLOT		,			// 0
		PISTOL_SLOT		,		// 1
		RIFLE_SLOT		,		// 2
		GRENADE_SLOT	,		// 3
		APPARATUS_SLOT	,		// 4
		OUTFIT_SLOT		,		// 5
};

void CUIGameDM::OnBuyMenu_Ok	()
{
	if (!m_bBuyEnabled) return;
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	game_cl_GameState::Player* Pl = m_game->local_player;
	if (!Pl) return;

	NET_Packet		P;
//	l_pPlayer->u_EventGen		(P,GEG_PLAYER_BUY_FINISHED,l_pPlayer->ID()	);
	l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
	P.w_u16(GAME_EVENT_PLAYER_BUY_FINISHED);
	//-------------------------------------------------------------------------------
	pCurPresetItems->clear();

	for (u8 s =0; s<6; s++)
	{
		u8 ItemID = pCurBuyMenu->GetWeaponIndex(SlotsToCheck[s]);
		if (ItemID == 0xff) continue;
		u16 SlotID = SlotsToCheck[s];
		if (SlotID == OUTFIT_SLOT) SlotID = APPARATUS_SLOT;
		s16	ID = (s16(SlotID) << 0x08) | s16(ItemID);
		pCurPresetItems->push_back(ID);
	}

	for (u8 i=0; i<pCurBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pCurBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
		s16	ID = (s16(SectID) << 0x08) | s16(ItemID);
		pCurPresetItems->push_back(ID);
	};	
	//-------------------------------------------------------------------------------
	P.w_s16		(s16(pCurBuyMenu->GetMoneyAmount()) - Pl->money_for_round);
	P.w_u8		(u8(pCurPresetItems->size()));
	for (s=0; s<pCurPresetItems->size(); s++)
	{
		P.w_s16((*pCurPresetItems)[s]);
	}
	//-------------------------------------------------------------------------------
	l_pPlayer->u_EventSend		(P);
};

bool		CUIGameDM::CanBeReady				()
{

	SetCurrentSkinMenu();

	SetCurrentBuyMenu();
	
	if (pCurBuyMenu && !pCurBuyMenu->IsShown())
		SetBuyMenuItems		();

	if (!m_bSkinSelected)
	{
		StartStopMenu(pCurSkinMenu);
		return false;
	};

	u8 res = 0xff;
	res &=	pCurBuyMenu->GetWeaponIndex(KNIFE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(PISTOL_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(RIFLE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(GRENADE_SLOT);

	if (res == 0xff || !pCurBuyMenu->CanBuyAllItems())
	{
		StartStopMenu(pCurBuyMenu);
		return false;
	};

	OnBuyMenu_Ok();
	return true;
};

//--------------------------------------------------------------------
CUIBuyWeaponWnd*		CUIGameDM::InitBuyMenu			(LPCSTR BasePriceSection, s16 Team)
{
	if (Team == -1)
	{
		Team = m_game->local_player->team;
	};

	std::string *pTeamSect = &m_aTeamSections[ModifyTeam(Team)];

	
	CUIBuyWeaponWnd* pMenu	= xr_new<CUIBuyWeaponWnd>	((LPCSTR)pTeamSect->c_str(), BasePriceSection);
	pMenu->SetSkin(0);
	return pMenu;
};

//--------------------------------------------------------------------
CUISkinSelectorWnd*		CUIGameDM::InitSkinMenu			(s16 Team)
{
	if (Team == -1)
	{
		Team = m_game->local_player->team;
	};

	std::string *pTeamSect = &m_aTeamSections[ModifyTeam(Team)];

	CUISkinSelectorWnd* pMenu	= xr_new<CUISkinSelectorWnd>	((char*)pTeamSect->c_str());
	
	return pMenu;
};
//--------------------------------------------------------------------
void		CUIGameDM::OnSkinMenu_Ok			()
{
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
//	l_pPlayer->u_EventGen		(P,GEG_PLAYER_CHANGE_SKIN,l_pPlayer->ID()	);

	l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
	P.w_u16(GAME_EVENT_PLAYER_CHANGE_SKIN);

	P.w_u8			(pCurSkinMenu->GetActiveIndex());
	l_pPlayer->u_EventSend		(P);
	//-----------------------------------------------------------------
	m_bSkinSelected = TRUE;

	if (pCurBuyMenu) 
		pCurBuyMenu->SetSkin(pCurSkinMenu->GetActiveIndex());
};

BOOL		CUIGameDM::CanCallBuyMenu			()
{
	CSpectator* pCurPlayer = dynamic_cast<CSpectator*> (Level().CurrentEntity());
	if (!pCurPlayer) return FALSE;

	return m_bBuyEnabled;
};

void		CUIGameDM::SetCurrentBuyMenu	()	
{
	pCurBuyMenu = pBuyMenuTeam0; 
	pCurPresetItems	= &PresetItemsTeam0;

	game_cl_GameState::Player* P = m_game->local_player;
	if (!P) return;
};

void		CUIGameDM::ClearBuyMenu			()
{
	if (!pCurBuyMenu) return;
	pCurBuyMenu->ClearSlots();
	pCurBuyMenu->ClearRealRepresentationFlags();
	pCurBuyMenu->InitInventory();
};

void		CUIGameDM::CheckItem			(PIItem pItem, PRESET_ITEMS* pPresetItems)
{
	if (!pItem || !pPresetItems) return;
	if (!pItem || pItem->getDestroy() || pItem->m_drop) return;

	u8 SlotID, ItemID;
	pCurBuyMenu->GetWeaponIndexByName(*pItem->cNameSect(), SlotID, ItemID);
	if (SlotID == 0xff || ItemID == 0xff) return;
	//-----------------------------------------------------
	pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
	//-----------------------------------------------------
	s16 BigID = (s16(SlotID) << 0x08) | s16(ItemID);
	s16 DesiredAddons = 0;
	PRESET_ITEMS_it It = pPresetItems->begin();
	PRESET_ITEMS_it Et = pPresetItems->end();
	for ( ; It != Et; ++It) 
	{
		if (BigID == ((*It)& 0xff1f))
		{
			DesiredAddons = ((*It)&0x00ff)>>5;
			pPresetItems->erase(It);
			break;
		}
	}
	//-----------------------------------------------------
	CWeapon* pWeapon = dynamic_cast<CWeapon*> (pItem);
	if (pWeapon)
	{
		if (pWeapon->ScopeAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetScopeName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsScopeAttached())
					pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonScope)
						pCurBuyMenu->SectionToSlot(SlotID, ItemID, false);
				}
			}
		};

		if (pWeapon->GrenadeLauncherAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetGrenadeLauncherName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsGrenadeLauncherAttached())
					pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)
						pCurBuyMenu->SectionToSlot(SlotID, ItemID, false);
				}
			}
		};

		if (pWeapon->SilencerAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetSilencerName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsSilencerAttached())
					pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonSilencer)
						pCurBuyMenu->SectionToSlot(SlotID, ItemID, false);
				}
			}
		};
	};
};

void		CUIGameDM::SetBuyMenuItems		()
{
	game_cl_GameState::Player* P = m_game->local_player;
	if (!P) return;
	//---------------------------------------------------------
	xr_vector <s16>			TmpPresetItems;
	PRESET_ITEMS_it		It = pCurPresetItems->begin();
	PRESET_ITEMS_it		Et = pCurPresetItems->end();
	for ( ; It != Et; ++It) 
	{
		TmpPresetItems.push_back(*It);
	};
	//---------------------------------------------------------
	ClearBuyMenu			();
	//---------------------------------------------------------
	pCurBuyMenu->IgnoreMoney(true);
	//---------------------------------------------------------
	CActor* pCurActor = dynamic_cast<CActor*> (Level().Objects.net_Find	(P->GameID));
	if (pCurActor)
	{
		//проверяем предметы которые есть у игрока

		/*
		TIItemSet::const_iterator	I = pCurActor->inventory().m_all.begin();
		TIItemSet::const_iterator	E = pCurActor->inventory().m_all.end();
		
		for ( ; I != E; ++I) 
		{
			CheckItem((*I), &TmpPresetItems);
		};
		*/

		//проверяем пояс
		TIItemList::const_iterator	IBelt = pCurActor->inventory().m_belt.begin();
		TIItemList::const_iterator	EBelt = pCurActor->inventory().m_belt.end();

		for ( ; IBelt != EBelt; ++IBelt) 
		{
			CheckItem((*IBelt), &TmpPresetItems);
		};

		//проверяем слоты
		TISlotArr::const_iterator	ISlot = pCurActor->inventory().m_slots.begin();
		TISlotArr::const_iterator	ESlot = pCurActor->inventory().m_slots.end();

		for ( ; ISlot != ESlot; ++ISlot) 
		{
			CheckItem((*ISlot).m_pIItem, &TmpPresetItems);
		};
	};
	//---------------------------------------------------------
	It = TmpPresetItems.begin();
	Et = TmpPresetItems.end();
	for ( ; It != Et; ++It) 
	{
		s16	ItemID = (*It);
		
		pCurBuyMenu->SectionToSlot(u8((ItemID&0xff00)>>0x08), u8(ItemID&0x00ff), false);
	};
	//---------------------------------------------------------
	pCurBuyMenu->IgnoreMoney(false);

	pCurBuyMenu->SetMoneyAmount(P->money_for_round);
	pCurBuyMenu->CheckBuyAvailabilityInSlots();
};

void CUIGameDM::SetTimeMsgCaption		(LPCSTR str)
{
		m_gameCaptions.setCaption(m_time_caption, str, TIME_MSG_COLOR, true);
}
