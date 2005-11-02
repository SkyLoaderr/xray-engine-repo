#pragma once

#include "UIWindow.h"

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIScrollView;
class CUIXml;

class CUIActorInfoWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
							CUIActorInfoWnd		();
	virtual void			Init				();
	virtual void			Show				(bool status);
	CUIScrollView&			DetailList			()				{return *UIDetailList;}
	CUIScrollView&			MasterList			()				{return *UIMasterList;}
	void					FillPointsDetail	(int idx);

protected:
	CUIFrameWindow*			UIInfoFrame;
	CUIFrameLineWnd*		UIInfoHeader;
	CUIFrameWindow*			UICharIconFrame;
	CUIFrameLineWnd*		UICharIconHeader;
	CUIAnimatedStatic*		UIAnimatedIcon;

	CUIWindow*				UICharacterWindow;
	CUICharacterInfo*		UICharacterInfo;

	CUIScrollView*			UIMasterList;
	CUIScrollView*			UIDetailList;
	void					FillPointsInfo				();
	void					FillReputationDetails		(CUIXml* xml, LPCSTR path);
};

class CUIActorStaticticHeader :public CUIWindow, public CUISelectable
{
	CUIActorInfoWnd*						m_actorInfoWnd;
protected:
	CUIStatic*		m_text;
	u32				m_stored_alpha;
public:
	CUIStatic*		m_num;
public:
					CUIActorStaticticHeader	(CUIActorInfoWnd* w);
	void			Init					(CUIXml* xml, LPCSTR path, int idx);
	virtual void	OnMouseDown				(bool left_button = true);
	virtual void	SetSelected				(bool b);
			void	SetText1				(LPCSTR str);

	int										m_index;
};

class CUIActorStaticticDetail :public CUIWindow
{
protected:
	CUIStatic*		m_text;
public:
	CUIStatic*		m_num1;
	CUIStatic*		m_num2;
public:
	void			Init					(CUIXml* xml, LPCSTR path, int idx);
			void	SetText1				(LPCSTR str);
			void	SetText2				(LPCSTR str);
			void	SetText3				(LPCSTR str);
};
