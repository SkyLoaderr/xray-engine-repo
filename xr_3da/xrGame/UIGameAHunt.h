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

public:
	virtual void		SetClGame				(game_cl_GameState* g);
	CUIGameAHunt								();
	virtual 			~CUIGameAHunt			();

	virtual	void		Init					();
			void		SetReinforcementCaption	(LPCSTR str);
			void		SetScoreCaption			(LPCSTR str);
			void		SetRoundResultCaption	(LPCSTR str);
			void		SetTodoCaption			(LPCSTR str);
			void		SetBuyMsgCaption		(LPCSTR str);
protected:
	shared_str				m_reinforcement_caption;
	shared_str				m_score_caption;
	shared_str				m_round_result_caption;		
	shared_str				m_todo_caption;			
	shared_str				m_buy_msg_caption;		

};
#endif // __XR_UIGAMEAHUNT_H__
