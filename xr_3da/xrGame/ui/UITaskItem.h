#pragma once
#include "UIListItem.h"
#include "UIWndCallback.h"

class CGameTask;
class CUIStatic;
class CUIButton;
struct SGameTaskObjective;

class CUITaskItem :public CUIListItem, public CUIWndCallback
{
	typedef		CUIListItem	inherited;
protected:
	CGameTask*		m_GameTask;
	int				m_TaskObjectiveIdx;
public:
					CUITaskItem				();
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
	CUIStatic*		m_descriptionStatic;
	CUI3tButton*	m_showLocationBtn;
	CUI3tButton*	m_switchDescriptionBtn;
	void			Init					();
public:	
					CUITaskRootItem			();
	virtual			~CUITaskRootItem		();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowLocationClicked	();
			void	OnSwitchDescriptionClicked();

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
					CUITaskSubItem			();
	virtual			~CUITaskSubItem			();
	virtual void	Update					();
	virtual void	SetGameTask				(CGameTask* gt, int obj_idx);
			void	OnShowPointerClicked	();

	virtual void	MarkSelected				(bool b);
};
