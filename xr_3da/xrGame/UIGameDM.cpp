#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"
#include "ui\UIBuyWeaponWnd.h"
#include "ui\UISkinSelector.h"

#define MSGS_OFFS 510

#define	TEAM0_MENU		"deathmatch_team0"

//--------------------------------------------------------------------
CUIGameDM::CUIGameDM(CUI* parent):CUIGameCustom(parent)
{
	ClearLists ();
	
	pBuyMenuTeam0	= NULL;
	pCurBuyMenu		= NULL;

	pSkinMenuTeam0	= NULL;
	pCurSkinMenu	= NULL;

	m_bBuyEnabled	= TRUE;
}
//--------------------------------------------------------------------
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
	pBuyMenuTeam0	= InitBuyMenu(0);
	pCurBuyMenu		= pBuyMenuTeam0;
	//-----------------------------------------------------------
	pSkinMenuTeam0	= InitSkinMenu(0);
	pCurSkinMenu	= pSkinMenuTeam0;
};
//--------------------------------------------------------------------

void	CUIGameDM::ClearLists ()
{
	for (u32 i=0; i<m_aFragsLists.size(); i++)
	{
		xr_delete(m_aFragsLists[i]);
	}
	m_aFragsLists.clear();

	for (u32 i=0; i<m_aPlayersLists.size(); i++)
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

	switch (Game().phase){
	case GAME_PHASE_PENDING: 
		for (u32 i=0; i<m_aPlayersLists.size(); i++)
		{
			m_aPlayersLists[i]->Update();
			if (!m_aPlayersLists[i]->IsShown()) 
			{
				m_aPlayersLists[i]->Show();
			};
		};
	break;
	case GAME_PHASE_INPROGRESS:
		if (uFlags&flShowFragList) 
		{
			for (u32 i=0; i<m_aFragsLists.size(); i++)
			{
				m_aFragsLists[i]->Update();
			};
//			pFragList->Update();
		}break;
	break;
	}
}
//--------------------------------------------------------------------

void CUIGameDM::Render()
{
	inherited::Render();

	switch (Game().phase){
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
	}

	if (Game().phase==GAME_PHASE_INPROGRESS){
		// switch pressed keys
		switch (dik){
		case DIK_TAB:	
			{
				SetFlag		(flShowFragList,TRUE);	
				return true;
			}break;
		case DIK_B:
			{
				if (!m_bBuyEnabled) break;

				SetCurrentBuyMenu	();

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

	if (Game().phase==GAME_PHASE_INPROGRESS){
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
void CUIGameDM::OnBuyMenu_Ok	()
{
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = dynamic_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P,GEG_PLAYER_BUY_FINISHED,l_pPlayer->ID()	);
	
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(KNIFE_SLOT));
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(PISTOL_SLOT));
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(RIFLE_SLOT));
	P.w_u8		(pCurBuyMenu->GetWeaponIndex(GRENADE_SLOT));

	P.w_u8		(pCurBuyMenu->GetBeltSize());
	
	for (u8 i=0; i<pCurBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pCurBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
		P.w_u8	(SectID);
		P.w_u8	(ItemID);
	};	

	l_pPlayer->u_EventSend		(P);
};

bool		CUIGameDM::CanBeReady				()
{
	u8 res = 0xff;

	SetCurrentBuyMenu();

	res &=	pCurBuyMenu->GetWeaponIndex(KNIFE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(PISTOL_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(RIFLE_SLOT);
	res &=	pCurBuyMenu->GetWeaponIndex(GRENADE_SLOT);
	res &=	~(pCurBuyMenu->GetBeltSize());

	if (res != 0xff) 
	{
		OnBuyMenu_Ok();
		return true;
	};

	StartStopMenu(pCurBuyMenu);
	return false;
};

//--------------------------------------------------------------------
void		CUIGameDM::FillDefItems		(const char* caSection, CUIBuyWeaponWnd* pMenu)
{
	if (!pSettings->section_exist(caSection)) return;

	if (!pSettings->line_exist(caSection, "default_items")) return;

	string4096			DefItems;
	string256			SingleItem;

	std::strcpy(DefItems, pSettings->r_string(caSection, "default_items"));
	u32 count	= _GetItemCount(DefItems);
	// ������ ��� ������ ��� ������, ����������� ��������, ������� � ������
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(DefItems, i, SingleItem);
		pMenu->MoveWeapon(SingleItem);
	};
};
//--------------------------------------------------------------------
CUIBuyWeaponWnd*		CUIGameDM::InitBuyMenu			(s16 Team)
{
	if (Team == -1)
	{
		Team = Game().local_player->team;
	};

	std::string *pTeamSect = &m_aTeamSections[ModifyTeam(Team)];

	
		CUIBuyWeaponWnd* pMenu	= xr_new<CUIBuyWeaponWnd>	((char*)pTeamSect->c_str());
		/*
	}
	else
	{
		pMenu->ReInitItems((char*)pTeamSect->c_str());
	};
	*/
	FillDefItems(pTeamSect->c_str(), pMenu);
	return pMenu;
};

//--------------------------------------------------------------------
CUISkinSelectorWnd*		CUIGameDM::InitSkinMenu			(s16 Team)
{
	if (Team == -1)
	{
		Team = Game().local_player->team;
	};

	std::string *pTeamSect = &m_aTeamSections[ModifyTeam(Team)];

	CUISkinSelectorWnd* pMenu	= xr_new<CUISkinSelectorWnd>	((char*)pTeamSect->c_str());
	
	return pMenu;
};