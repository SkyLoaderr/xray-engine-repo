#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUICellContainer;
class CUIScrollBar;
class CUIStatic;
class CUICellItem;
class CUIDragItem;

struct CUICell{
							CUICell					()						{Clear();}
		CUICellItem*		m_item;
		void				SetItem					(CUICellItem* itm)		{m_item = itm;}
		bool				Empty					()						{return m_item == NULL;}
		void				Clear					()						{SetItem(NULL);}
		bool				operator ==				(const CUICell& C)		{return (m_item == C.m_item);}
};

typedef xr_vector<CUICell>			UI_CELLS_VEC;
typedef UI_CELLS_VEC::iterator		UI_CELLS_VEC_IT;

class CUIDragDropListEx :public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow inherited;

	enum{	
		flGroupSimilar		=	(1<<0),
		flAutoGrow			=	(1<<1)
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
	void					OnItemDBClick			(CUIWindow* w, void* pData);
	
public:
	static CUIDragItem*		m_drag_item;
							CUIDragDropListEx	();
	virtual					~CUIDragDropListEx	();
				void		Init				(float x, float y, float w, float h);

	typedef					fastdelegate::FastDelegate1<CUICellItem*, bool>		DRAG_DROP_EVENT;

	DRAG_DROP_EVENT			m_f_item_drop;
	DRAG_DROP_EVENT			m_f_item_start_drag;
	DRAG_DROP_EVENT			m_f_item_db_click;
	DRAG_DROP_EVENT			m_f_item_selected;
	DRAG_DROP_EVENT			m_f_item_rbutton_click;

	const	Ivector2&		CellsCapacity		();
			void			SetCellsCapacity	(const Ivector2 c);
	 const	Ivector2&		CellSize			();
			void			SetCellSize			(const Ivector2 new_sz);
			int				ScrollPos			();
			void			ReinitScroll		();
			void			GetClientArea		(Frect& r);
			Fvector2		GetDragItemPosition	();

			void			SetAutoGrow			(bool b);
			bool			IsAutoGrow			();
			void			SetGrouping			(bool b);
			bool			IsGrouping			();
public:
			// items management
			void			SetItem				(CUICellItem* itm); //auto
			void			SetItem				(CUICellItem* itm, Fvector2 abs_pos);  // start at cursor pos
			void			SetItem				(CUICellItem* itm, Ivector2 cell_pos); // start at cell
			CUICellItem*	RemoveItem			(CUICellItem* itm, bool force_root);
			void			CreateDragItem		(CUICellItem* itm);
			void			DestroyDragItem		();
public:
	//UIWindow overriding
	virtual		void		Draw				();
	virtual		void		Update				();
	virtual		bool		OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual		void		SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);

};

class CUICellContainer :public CUIWindow
{
	friend class CUIDragDropListEx;

private:
	typedef CUIWindow inherited;
	ref_shader					hShader;  //ownerDraw
	ref_geom					hGeom;	

protected:
	CUIDragDropListEx*			m_pParentDragDropList;

	Ivector2					m_cellsCapacity;			//count		(col,	row)
	Ivector2					m_cellSize;					//pixels	(width, height)
	UI_CELLS_VEC				m_cells;

	void						GetTexUVLT			(Fvector2& uv, u32 col, u32 row);
	void						ReinitSize			();
	u32							GetCellsInRange		(const Irect& rect, UI_CELLS_VEC& res);

public:							
								CUICellContainer	(CUIDragDropListEx* parent);
	virtual						~CUICellContainer	();
protected:
	virtual		void			Draw				();

	IC const	Ivector2&		CellsCapacity		()								{return m_cellsCapacity;};	
				void			SetCellsCapacity	(const Ivector2& c);
	IC const	Ivector2&		CellSize			()								{return m_cellSize;};	
				void			SetCellSize			(const Ivector2& new_sz);
				Ivector2		TopVisibleCell		();
				CUICell&		GetCellAt			(const Ivector2& pos);
				Ivector2		PickCell			(const Fvector2& abs_pos);
				Ivector2		GetItemPos			(CUICellItem* itm);
				Ivector2		FindFreeCell		(const Ivector2& size);
				bool			IsRoomFree			(const Ivector2& pos, const Ivector2& size);
				
				bool			AddSimilar			(CUICellItem* itm);
				CUICellItem*	FindSimilar			(CUICellItem* itm);

				void			PlaceItemAtPos		(CUICellItem* itm, Ivector2& cell_pos);
				CUICellItem*	RemoveItem			(CUICellItem* itm, bool force_root);
				bool			ValidCell			(const Ivector2& pos) const;

				void			Grow				();
				void			Shrink				();

};

#include "UIDialogWnd.h"
class CTestDragDropWnd :public CUIDialogWnd
{
public:
						CTestDragDropWnd		();
	virtual				~CTestDragDropWnd		(){};

	CUIDragDropListEx*	m_list1;
	CUIDragDropListEx*	m_list2;

	bool		xr_stdcall		OnItemDrop			(CUICellItem* itm);
	bool		xr_stdcall		OnItemStartDrag		(CUICellItem* itm);
	bool		xr_stdcall		OnItemDbClick		(CUICellItem* itm);
	bool		xr_stdcall		OnItemSelected		(CUICellItem* itm);
	bool		xr_stdcall		OnItemRButtonClick	(CUICellItem* itm);

};