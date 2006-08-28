#pragma once

#include "uiwindow.h"
#include "uibutton.h"
#include "../script_export_space.h"

class CUIProgressBar : public CUIWindow
{
	friend class CUIXmlInit;
protected:
	bool				m_bIsHorizontal;

	float				m_ProgressPos;
	float				m_MinPos;
	float				m_MaxPos;

	//текущий состояние полосы в пикселях
	float				m_CurrentLength;

	bool				m_bBackgroundPresent;
	Fvector2			m_BackgroundOffset;

	void				UpdateProgressBar();

public:
	bool				m_bUseColor;
	Fcolor				m_minColor;
	Fcolor				m_maxColor;

public:
	CUIStatic			m_UIProgressItem;
	CUIStatic			m_UIBackgroundItem;


						CUIProgressBar				();
	virtual				~CUIProgressBar				();


	virtual void		Init						(float x, float y, float width, float height, bool bIsHorizontal);

	void				SetRange					(float _Min, float _Max)	{ m_MinPos = _Min;  m_MaxPos = _Max; UpdateProgressBar();}
//.	void				GetRange					(float& _Min, float& _Max)	{ _Min = m_MinPos;  _Max = m_MaxPos;}

	float				GetRange_min				() 							{ return  m_MinPos;}
	float				GetRange_max				() 							{ return  m_MaxPos;}

	void				SetProgressPos				(float _Pos)				{ m_ProgressPos = _Pos; UpdateProgressBar();}
	float				GetProgressPos				()							{ return m_ProgressPos; }


	virtual void		Draw						();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIProgressBar)
#undef script_type_list
#define script_type_list save_type_list(CUIProgressBar)
