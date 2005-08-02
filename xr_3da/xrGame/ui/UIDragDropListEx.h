#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUICellContainer;
class CUIScrollBar;
class CUIStatic;
class CUICellItem;

struct CUICell{
							CUICell		()					{Clear();}
		CUICellItem*		m_item;
		void				SetItem		(CUICellItem* itm)	{m_item =  itm;}
		bool				Empty		()					{return m_item == NULL;}
		void				Clear		()					{SetItem(NULL);}
		bool operator ==	(const CUICell& C)				{return (m_item == C.m_item);}
};

typedef xr_vector<CUICell>			UI_CELLS_VEC;
typedef UI_CELLS_VEC::iterator		UI_CELLS_VEC_IT;

class CUIDragDropListEx :public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow inherited;

	enum{	
		flGroupSimilar		=	(1<<0),
	};
	Flags8					m_flags;
	CUIStatic*				m_background;
	CUICellItem*			m_selected_item;
protected:
	
	CUICellContainer*		m_container;
	CUIScrollBar*			m_vScrollBar;
	void					OnScrollV				(CUIWindow* w, void* pData);
	void					OnItemStartDragging		(CUIWindow* w, void* pData);
	void					OnItemDrop				(CUIWindow* w, void* pData);
	void					OnItemSelected			(CUIWindow* w, void* pData);
	void					OnItemRButtonClick		(CUIWindow* w, void* pData);
public:
							CUIDragDropListEx	();
	virtual					~CUIDragDropListEx	();
				void		Init				(float x, float y, float w, float h);

	const	Ivector2&		CellsCapacity		();
			void			SetCellsCapacity	(const Ivector2 c);
	 const	Ivector2&		CellSize			();
			void			SetCellSize			(const Ivector2 new_sz);
			int				ScrollPos			();
			void			ReinitScroll		();
			void			AddItem				(CUICellItem*itm ,int x, int y);
			void			GetClientArea		(Frect& r);
public:
	//UIWindow overriding
	virtual		void		Draw				();
	virtual		void		Update				();
	virtual		bool		OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual		void		SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);

};

class CUICellContainer :public CUIWindow
{
private:
	typedef CUIWindow inherited;
	ref_shader					hShader;  //ownerDraw
	ref_geom					hGeom;	

	Flags8						m_flags; //not used still
protected:
	CUIDragDropListEx*			m_pParentDragDropList;

	Ivector2					m_cellsCapacity;			//count		(col,	row)
	Ivector2					m_cellSize;					//pixels	(width, height)
	UI_CELLS_VEC				m_cells;

	void						GetTexUVLT(Fvector2& uv, u32 col, u32 row);
	void						ReinitSize();
public:
								CUICellContainer			(CUIDragDropListEx* parent);
	virtual						~CUICellContainer			();
	virtual		void			Draw				();

	IC const	Ivector2&		CellsCapacity		()								{return m_cellsCapacity;};	
				void			SetCellsCapacity	(const Ivector2 c);
	IC const	Ivector2&		CellSize			()								{return m_cellSize;};	
				void			SetCellSize			(const Ivector2 new_sz);
				Ivector2		TopVisibleCell		();
		
				CUICell&		GetCellAt			(int x, int y);
				u32				GetCellsInRange		(const Irect rect, UI_CELLS_VEC& res);

};

#include "UIDialogWnd.h"
class CTestDragDropWnd :public CUIDialogWnd
{
public:
						CTestDragDropWnd		();
	virtual				~CTestDragDropWnd		();

	CUIDragDropListEx*	m_list1;
	CUIDragDropListEx*	m_list2;
};