#ifndef __XR_UIBUYMENU_H__
#define __XR_UIBUYMENU_H__
#pragma once

class CUIBuyMenu{
	class CMenuItem;
	DEFINE_VECTOR		(CMenuItem*,MIVec,MIIt);
	class CMenuItem{
	public:
		LPSTR			caption;
		DWORD			tag;
		MIVec			items;
	public:
		CMenuItem		(LPCSTR text, DWORD t)
		{
			caption		= xr_strdup(text);
			tag			= t;
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
		void			OnItemDraw(CGameFont* F, int num)
		{
			if (items.empty()){
				F->OutNext	("%-2d: %-32s %6d$",num,caption,tag);
			}else{
				F->OutNext	("%-2d: %-32s >",num,caption);
			}
		}
	};
	CMenuItem*			menu_root;	
	CMenuItem*			menu_active;

	int					menu_offs;
public:
						CUIBuyMenu			();
	virtual				~CUIBuyMenu			();

	void				Load				();

	void				Render				();
	void				OnFrame				();

	bool				OnKeyboardPress		(int dik);
	bool				OnKeyboardRelease	(int dik);
	bool				OnMouseMove			(int dx, int dy);
};

#endif // __XR_UIBUYMENU_H__
