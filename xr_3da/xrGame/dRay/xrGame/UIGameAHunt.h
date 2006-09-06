#pragma once

#include "UIGameCustom.h"
#include "UIGameTDM.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UISpawnWnd.h"

#include "ui/UIBuyWnd.h"
#include "ui/UITextBanner.h"

// refs 
class CUIAHuntFragList;
class CUIAHuntPlayerList;
class CUIBuyWnd;
class game_cl_ArtefactHunt;
class CUIProgressShape;

class CUIGameAHunt: public CUIGameTDM
{
private:
	game_cl_ArtefactHunt * m_game;
	typedef CUIGameTDM inherited;


public:
	CUIProgressShape*	m_pReinforcementInidcator;

public:
	virtual void		SetClGame				(game_cl_GameState* g);
	CUIGameAHunt								();
	virtual 			~CUIGameAHunt			();

	virtual	void		Init					();
			void		SetTodoCaption			(LPCSTR str);
			void		SetBuyMsgCaption		(LPCSTR str);

	virtual void		Render					();
	virtual void		OnFrame					();

protected:
	shared_str				m_todo_caption;			
	shared_str				m_buy_msg_caption;		

};
