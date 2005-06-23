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
class CUIPdaCommunication;
class CUIEncyclopediaWnd;
class CUIDiaryWnd;
class CUIActorInfoWnd;
class CUIStalkersRankingWnd;

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

	enum EPdaTabs
	{
		eptEvents = 0,
		eptComm,
		eptMap,
		eptEncyclopedia,
		eptActorStatistic,
		eptRanking
	};
protected:
	// ���������
	CUIStatic*				UIMainPdaFrame;

	// ������� �������� ������
	CUIWindow*				m_pActiveDialog;
public:
	// ���������� PDA
	CUIMapWnd*				UIMapWnd;
	CUIPdaCommunication*	UIPdaCommunication;
	CUIEncyclopediaWnd*		UIEncyclopediaWnd;
	CUIDiaryWnd*			UIDiaryWnd;
	CUIActorInfoWnd*		UIActorInfo;
	CUIStalkersRankingWnd*	UIStalkersRanking;
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
	
	// ����������� ��� ������������� ���� PDA ���������:
	// ����� ����� ������������� �����
//	typedef enum { PDA_MAP_SET_ACTIVE_POINT = 8010 } E_MESSAGE;
	// ������������� �� ����� � ��������������� �� �������� �����
	void					FocusOnMap			(const int x, const int y, const int z);
	void					SetActiveSubdialog	(EPdaSections section, ARTICLE_ID addiotionalValue = "");
	virtual bool			StopAnyMove			(){return false;}
};
