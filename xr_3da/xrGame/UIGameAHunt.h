#ifndef __XR_UIGAMEAHUNT_H__
#define __XR_UIGAMEAHUNT_H__
#pragma once

#include "UIGameCustom.h"
#include "UIGameTDM.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapWnd.h"
#include "ui/UISpawnWnd.h"

#include "ui/UIBuyWeaponWnd.h"
#include "ui/UITextBanner.h"

// refs 
class CUIAHuntFragList;
class CUIAHuntPlayerList;
class CUIBuyWeaponWnd;
class game_cl_ArtefactHunt;

class CUIGameAHunt: public CUIGameTDM
{
private:
	game_cl_ArtefactHunt * m_game;
	typedef CUIGameTDM inherited;

protected:
	/*
	CUIAHuntFragList*		pFragListT1;
	CUIAHuntFragList*		pFragListT2;
	CUIAHuntPlayerList*		pPlayerListT1;
	CUIAHuntPlayerList*		pPlayerListT2;

	CUISpawnWnd*		pUITeamSelectWnd;

	CUIBuyWeaponWnd*	pBuyMenu;
	*/
public:
	virtual void		SetClGame				(game_cl_GameState* g);
	CUIGameAHunt								();
	virtual 			~CUIGameAHunt			();

	virtual	void		Init					();

	virtual void		OnFrame					();

	virtual void		OnObjectEnterTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone);
	virtual void		OnObjectLeaveTeamBase	(CObject *tpObject, CTeamBaseZone* pTeamBaseZone);

	virtual BOOL		CanCallBuyMenu			();

	virtual bool		CanBeReady				();
	
			void		SetScoreCaption			(LPCSTR str);
			void		SetRoundResultCaption	(LPCSTR str);
			void		SetTodoCaption			(LPCSTR str);
			void		SetBuyMsgCaption		(LPCSTR str);
protected:
	ref_str				m_score_caption;		
	ref_str				m_round_result_caption;		
	ref_str				m_todo_caption;			
	ref_str				m_buy_msg_caption;		

};
#endif // __XR_UIGAMEAHUNT_H__
