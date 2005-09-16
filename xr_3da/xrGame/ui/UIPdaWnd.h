// CUIPdaWnd.h:  ������ PDA
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
// �������� � �������� ������ PDA
///////////////////////////////////////

class CUIPdaWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd	inherited;
protected:
	//�������� ������������� ����������
	CUIFrameLineWnd*		UIMainButtonsBackground;
	CUIFrameLineWnd*		UITimerBackground;

	// �������� ���������� ���
	CUIButton*				UIOffButton;

	// ������ PDA
	CUITabControl*			UITabControl;

	// ���������� ������� �����
	void					UpdateDateTime		();

protected:
	// ���������
	CUIStatic*				UIMainPdaFrame;

	// ������� �������� ������
	CUIWindow*				m_pActiveDialog;
	EPdaTabs				m_pActiveSection;
public:
	// ���������� PDA
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

	virtual void 			Draw				();
	virtual void 			Update				();
	virtual void 			Show				();
	virtual void 			Hide				();
	virtual bool			OnMouse				(float x, float y, EUIMessages mouse_action) {CUIDialogWnd::OnMouse(x,y,mouse_action);return true;} //always true because StopAnyMove() == false
			void			OnNewArticleAdded	();
			void			OnContactsChanged	();
	
	void					SetActiveSubdialog	(EPdaTabs section);
	virtual bool			StopAnyMove			(){return false;}
};
