// CUIPdaWnd.h:  диалог PDA
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIPdaAux.h"
#include "../encyclopedia_article_defs.h"

class CInventoryOwner;
class CUIFrameLineWnd;
class CUIButton;
class CUITabControl;
class CUIStatic;
class CUIMapWnd;
//class CUIPdaCommunication;
class CUIEncyclopediaWnd;
class CUIDiaryWnd;
class CUIActorInfoWnd;
class CUIStalkersRankingWnd;
class CUIEventsWnd;
class CUIPdaContactsWnd;
//////////////////////////////////////////////////////////////////////////

extern const char * const PDA_XML;

///////////////////////////////////////
// Подложка и основные кнопки PDA
///////////////////////////////////////

class CUIPdaWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd	inherited;
protected:
	//элементы декоративного интерфейса
	CUIFrameLineWnd*		UIMainButtonsBackground;
	CUIFrameLineWnd*		UITimerBackground;

	// Кнопочка выключения ПДА
	CUIButton*				UIOffButton;

	// кнопки PDA
	CUITabControl*			UITabControl;

	// Установить игровое время
	void					UpdateDateTime		();

protected:
	// Бэкграунд
	CUIStatic*				UIMainPdaFrame;

	// Текущий активный диалог
	CUIWindow*				m_pActiveDialog;
	EPdaTabs				m_pActiveSection;
public:
	// Поддиалоги PDA
	CUIMapWnd*				UIMapWnd;
//	CUIPdaCommunication*	UIPdaCommunication;
	CUIPdaContactsWnd*		UIPdaContactsWnd;
	CUIEncyclopediaWnd*		UIEncyclopediaWnd;
	CUIDiaryWnd*			UIDiaryWnd;
	CUIActorInfoWnd*		UIActorInfo;
	CUIStalkersRankingWnd*	UIStalkersRanking;
	CUIEventsWnd*			UIEventsWnd;
public:
							CUIPdaWnd			();
	virtual					~CUIPdaWnd			();

	virtual void 			Init				();

	virtual void 			SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);

//	virtual void 			Draw				();
	virtual void 			Update				();
	virtual void 			Show				();
	virtual void 			Hide				();
			void			OnNewArticleAdded	();
			void			OnContactsChanged	();
	
	void					SetActiveSubdialog	(EPdaTabs section);
	virtual bool			StopAnyMove			(){return false;}
};
