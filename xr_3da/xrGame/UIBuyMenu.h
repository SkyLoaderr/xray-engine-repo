#ifndef __XR_UIBUYMENU_H__
#define __XR_UIBUYMENU_H__
#pragma once

class CUIBuyMenu{
	class CMenuItem;
	DEFINE_VECTOR		(CMenuItem*,MIVec,MIIt);
	typedef void 		(*OnExecuteEvent)		(CMenuItem* sender);
	class CMenuItem{
	public:
		LPSTR			caption;
		int				tag;
		MIVec			items;
		OnExecuteEvent	OnExecute;
		CMenuItem*		m_Parent;
	public:
		CMenuItem		(CMenuItem* parent, LPCSTR text, DWORD t, OnExecuteEvent event=0)
		{
			m_Parent	= parent;
			caption		= xr_strdup(text);
			tag			= t;
			OnExecute	= event;
		}
		~CMenuItem		()
		{
			xr_free		(caption);
			for (MIIt it=items.begin(); it!=items.end(); it++)
				_DELETE	(*it);
		}
		void			AppendItem(CMenuItem* I)
		{
			items.push_back(I);
		}
		void			OnItemDraw(CGameFont* F, int num, int col)
		{
			switch(col){
			case 0: F->OutNext						("%d. %s",num,caption);	break;
			case 1: if (items.empty()) F->OutNext	("%d",tag);			break;
			default: THROW;
			}
		}
		IC BOOL			IsMenu				()			{return !items.empty();}
		IC CMenuItem*	GetItem				(int id)	
		{
			id--;
			if (-1==id) return m_Parent;
			if (id<items.size()) return items[id];
			return 0;
		}
		IC void			Execute				()			{if (OnExecute) OnExecute(this);}
	};
	CMenuItem*			menu_root;	
	CMenuItem*			menu_active;

	static void 		BackItem			(CMenuItem* sender);
	static void 		BuyItem				(CMenuItem* sender);
	int					menu_offs;
	int					menu_offs_col[2];
	void				ParseMenu			(CInifile* ini, CMenuItem* root, LPCSTR sect);
public:
						CUIBuyMenu			();
	virtual				~CUIBuyMenu			();

	void				Load				();

	void				Render				();
	void				OnFrame				();

	bool				OnKeyboardPress		(int dik);
	bool				OnKeyboardRelease	(int dik);
	bool				OnMouseMove			(int dx, int dy);

	void				OnActivate			(){menu_active=menu_root;}
	void				OnDeactivate		(){;}
};

#endif // __XR_UIBUYMENU_H__
