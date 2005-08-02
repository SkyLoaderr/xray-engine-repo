#pragma once
#include "UIListItem.h"
#include "UIWndCallback.h"

class CGameTask;
class CUIStatic;
class CUIButton;
struct SGameTaskObjective;
class CUIEventsWnd;
class CUIEditBox;

class CUITaskItem :public CUIListItem, public CUIWndCallback
{
	typedef		CUIListItem	inherited;
protected:
	CUIEventsWnd*	m_EventsWnd;
	CGameTask*		m_GameTask;
	int				m_TaskObjectiveIdx;
	void			OnClick					();
	void			Init					();
public:
					CUITaskItem				(CUIEventsWnd* w);
	virtual			~CUITaskItem			();
	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
					
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
	virtual void	Update					();

	CGameTask*		GameTask				()	{return m_GameTask;}
	int				ObjectiveIdx			()	{return m_TaskObjectiveIdx;}
	SGameTaskObjective*	Objective			();
};

class CUITaskRootItem :public CUITaskItem
{
	typedef		CUITaskItem	inherited;
protected:
	CUIStatic*		m_taskImage;
	CUIStatic*		m_captionStatic;
//	CUIStatic*		m_descriptionStatic;
	CUI3tButton*	m_showLocationBtn;
	CUI3tButton*	m_switchDescriptionBtn;
	u32				m_defTextColor;
	u32				m_defColor;
	void			Init					();
public:	
					CUITaskRootItem			(CUIEventsWnd* w);
	virtual			~CUITaskRootItem		();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowLocationClicked	();
			void	OnSwitchDescriptionClicked();

	virtual void	MarkSelected			(bool b);
	virtual bool	OnDbClick				();
};

class CUITaskSubItem :public CUITaskItem
{
	typedef			CUITaskItem	inherited;
	u32				m_defTextColor;
	u32				m_defColor;
protected:
	CUI3tButton*	m_showPointerBtn;
	CUIStatic*		m_descriptionStatic;
	CUIStatic*		m_stateStatic;

	void			Init					();

public:	
					CUITaskSubItem			(CUIEventsWnd* w);
	virtual			~CUITaskSubItem			();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowPointerClicked	();

	virtual void	MarkSelected			(bool b);
	virtual bool	OnDbClick				();
};

class CUIUserTaskItem :public CUITaskItem
{
	typedef			CUITaskItem	inherited;
	u32				m_defTextColor;
protected:
	CUI3tButton*	m_showPointerBtn;
	CUI3tButton*	m_showLocationBtn;
	CUIEditBox*		m_descriptionStatic;
	CUIStatic*		m_image;

	void			Init					();

public:	
					CUIUserTaskItem			(CUIEventsWnd* w);
	virtual			~CUIUserTaskItem			();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowLocationClicked	();
			void	OnShowPointerClicked	();
			void	OnDescriptionChanged	();
	virtual bool	OnDbClick				()	{return true;};
	virtual void	MarkSelected			(bool b);
};
