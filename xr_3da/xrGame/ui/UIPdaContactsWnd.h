
#pragma once

#include "UIWindow.h"


extern const char * const PDA_CONTACTS_HEADER_SUFFIX;


class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIStatic;
class CUIAnimatedStatic;
class CUIListWnd;
class CUIScrollView;
class CPda;

class CUIPdaContactsWnd: public CUIWindow  
{
private:
	typedef CUIWindow inherited;
	enum		{flNeedUpdate  =(1<<0),};
	Flags8		m_flags;
public:
	CUIPdaContactsWnd();
	virtual ~CUIPdaContactsWnd();

			void Init();


	virtual void SendMessage	(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Update			();

	virtual void Show			(bool status);

	void AddContact				(CPda* pda);
	void RemoveContact			(CPda* pda);
	void RemoveAll				();
	void Reload					();

	CUIScrollView*					UIListWnd;

protected:

	// Элементы оформления
	CUIFrameWindow*				UIFrameContacts;
	CUIFrameLineWnd*			UIContactsHeader;
	CUIFrameWindow*				UIRightFrame;
	CUIFrameLineWnd*			UIRightFrameHeader;
	CUIStatic*					UIArticleHeader;
	CUIAnimatedStatic*			UIAnimation;
};