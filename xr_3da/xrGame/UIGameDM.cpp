#include "stdafx.h"
#include "UIGameDM.h"

#include "UIDMPlayerList.h"
#include "UIDMFragList.h"
#include "ui\UIBuyWeaponWnd.h"

#define MSGS_OFFS 510

#define	TEAM0_MENU		"deathmatch_team0"

//--------------------------------------------------------------------
CUIGameDM::CUIGameDM(CUI* parent):CUIGameCustom(parent)
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
	pBuyMenu = NULL;
}
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

	xr_delete(pBuyMenu);
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
				if (!pBuyMenu) InitBuyMenu();
				StartStopMenu(pBuyMenu);
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
	
	P.w_u8		(pBuyMenu->GetWeaponIndex(KNIFE_SLOT));
	P.w_u8		(pBuyMenu->GetWeaponIndex(PISTOL_SLOT));
	P.w_u8		(pBuyMenu->GetWeaponIndex(RIFLE_SLOT));
	P.w_u8		(pBuyMenu->GetWeaponIndex(GRENADE_SLOT));

	P.w_u8		(pBuyMenu->GetBeltSize());
	
	for (u8 i=0; i<pBuyMenu->GetBeltSize(); i++)
	{
		u8 SectID, ItemID;
		pBuyMenu->GetWeaponIndexInBelt(i, SectID, ItemID);
		P.w_u8	(SectID);
		P.w_u8	(ItemID);
	};	

	l_pPlayer->u_EventSend		(P);
};

bool		CUIGameDM::CanBeReady				()
{
	if (!pBuyMenu) InitBuyMenu();

	u8 res = 0xff;

	res &=	pBuyMenu->GetWeaponIndex(KNIFE_SLOT);
	res &=	pBuyMenu->GetWeaponIndex(PISTOL_SLOT);
	res &=	pBuyMenu->GetWeaponIndex(RIFLE_SLOT);
	res &=	pBuyMenu->GetWeaponIndex(GRENADE_SLOT);
	res &=	~(pBuyMenu->GetBeltSize());

	if (res != 0xff) return true;

	StartStopMenu(pBuyMenu);
	return false;
};

//--------------------------------------------------------------------
void		CUIGameDM::FillDefItems		(const char* caSection)
{
	if (!pSettings->section_exist(caSection)) return;

	if (!pSettings->line_exist(caSection, "default_items")) return;

	string4096			DefItems;
	string256			SingleItem;

	std::strcpy(DefItems, pSettings->r_string(caSection, "default_items"));
	u32 count	= _GetItemCount(DefItems);
	// теперь для каждое имя оружия, разделенные запятыми, заносим в массив
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(DefItems, i, SingleItem);
		pBuyMenu->MoveWeapon(SingleItem);
	};

	OnBuyMenu_Ok();
};
//--------------------------------------------------------------------
void		CUIGameDM::InitBuyMenu			(s16 Team)
{
	if (Team == -1)
	{
		Team = Game().local_player->team;
	};

	std::string *pTeamSect = &m_aTeamSections[ModifyTeam(Team)];

	if (!pBuyMenu)
	{
		pBuyMenu	= xr_new<CUIBuyWeaponWnd>	((char*)pTeamSect->c_str());
	}
	else
	{
		pBuyMenu->ReInitItems((char*)pTeamSect->c_str());
	};
	FillDefItems(pTeamSect->c_str());
};