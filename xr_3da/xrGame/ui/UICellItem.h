#pragma once

#include "UIStatic.h"
#include "UIDialogWnd.h"

class CUIFlyingItem;
class CUICellItem :public CUIStatic
{
private:
	typedef		CUIStatic	inherited;
protected:
	enum{
		flMultipleStorage	= (1<<0),
		flFlying			= (1<<1),
	};
	DEFINE_VECTOR(void*,DATA_VEC, DATA_VEC_IT);
	DATA_VEC				m_pData;
public:
							CUICellItem				(void* pData);
	virtual					~CUICellItem			();

//	virtual		void*		GetData					()											{return m_pData;}
//	virtual		void		SetData					(void* pData)								{m_pData = pData;}
	virtual		void		OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual		void		Draw					();
	virtual		void		Update					()						{inherited::Update();};

	virtual		bool		EqualTo					(CUICellItem*)			{return false;};
	virtual		Ivector2	GetCellsSize			()						{return Ivector2().set(1,1);}; //size in grid

	virtual  CUIFlyingItem*	CreateFlyingItem		();

	virtual		void		InitInternals			(); //tmp

};

class CUIFlyingItem: public CUIDialogWnd
{
private:
	typedef		CUIDialogWnd	inherited;
	CUIStatic				m_static;
	CUICellItem*			m_pParent;
public:
							CUIFlyingItem(CUICellItem* parent);
	virtual		void		Init(const ref_shader& sh, const Frect& rect, const Frect& text_rect);
	virtual					~CUIFlyingItem();
	virtual		void		OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual		void		Draw					();
	void	__stdcall		OnMouseMoved			(Fvector2 delta);
};