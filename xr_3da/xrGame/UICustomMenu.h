#ifndef __XR_UICUSTOMMENU_H__
#define __XR_UICUSTOMMENU_H__
#pragma once

class					CUICustomMenuItem;
class					CUICustomMenu;
DEFINE_VECTOR			(CUICustomMenuItem*,MIVec,MIIt);

typedef void 			(*OnExecuteEvent)	(CUICustomMenuItem* sender);
typedef void			(*OnItemDrawEvent)	(CGameFont* F, int num, int col);

class CUICustomMenuItem{
	CUICustomMenu*		m_Owner;
	CUICustomMenuItem*	m_Parent;
	MIVec				items;
	OnExecuteEvent		OnExecute;
	OnItemDrawEvent		OnItemDraw;
public:
	LPSTR				title;
	LPSTR				caption;
	LPSTR				value0;
	LPSTR				value1;
public:
	CUICustomMenuItem	(CUICustomMenu* owner, CUICustomMenuItem* parent, LPCSTR text, LPCSTR val0, LPCSTR val1=0, OnExecuteEvent exec=0, OnItemDrawEvent draw=0)
	{
		m_Parent		= parent;
		title			= 0;
		caption			= text?xr_strdup(text):0;
		value0			= val0?xr_strdup(val0):0;
		value1			= val1?xr_strdup(val1):0;
		m_Owner			= owner;
		OnExecute		= exec;
		OnItemDraw		= draw;
	}
	~CUICustomMenuItem	()
	{
		_FREE			(value0);
		_FREE			(value1);
		_FREE			(caption);
		_FREE			(title);
		for (MIIt it=items.begin(); it!=items.end(); it++)
			_DELETE		(*it);
	}
	IC CUICustomMenu*	Owner				(){return m_Owner;}
	void				SetTitle			(LPCSTR text)
	{
		_FREE			(title);
		title			= text?xr_strdup(text):0;
	}
	void				AppendItem			(CUICustomMenuItem* I)
	{
		items.push_back	(I);
	}
	void				DrawItem			(CGameFont* F, int num, int col)
	{
		if (OnItemDraw)	{
			OnItemDraw	(F,num,col);
		}else{
			switch(col){
			case 0:						F->OutNext	("%d. %s",num,caption);	break;
			case 1: if (!HasChildren())	F->OutNext	("%s",value0);			break;
			default: THROW;
			}
		}
	}
	IC BOOL					HasChildren			()			{return !items.empty();}
	IC CUICustomMenuItem*	Parent				()			{return m_Parent;}
	IC MIIt					FirstItem			()			{return items.begin();}
	IC MIIt					LastItem			()			{return items.end();}
	IC u32					ItemCount			()			{return items.size();}
	IC CUICustomMenuItem*	GetItem				(int id)	
	{
		id--;
		if (-1==id) return m_Parent;
		if (id<(int)(items.size())) return items[id];
		return 0;
	}
	void				Execute				();
};

class CUICustomMenu{
	BOOL				bVisible;
protected:
	CUICustomMenuItem*	menu_root;	
	CUICustomMenuItem*	menu_active;
public:
	LPVOID				m_Owner; 
public:
						CUICustomMenu		();
	virtual				~CUICustomMenu		();

	virtual void		Load				(LPCSTR ini, LPCSTR start_sect, LPVOID owner, OnExecuteEvent exec=0);

	virtual void		Render				()=0;
	virtual void		OnFrame				()=0;

	virtual bool		OnKeyboardPress		(int dik)=0;
	virtual bool		OnKeyboardRelease	(int dik)=0;
	virtual bool		OnMouseMove			(int dx, int dy)=0;

	void				Show				(){bVisible=TRUE; menu_active=menu_root;}
	void				Hide				(){bVisible=FALSE;}
	IC BOOL				Visible				(){return bVisible;}
};

CUICustomMenuItem*		UIParseMenu			(CUICustomMenu* owner, CInifile* ini, CUICustomMenuItem* root, LPCSTR sect, OnExecuteEvent exec=0, OnItemDrawEvent draw=0);
CUICustomMenuItem*		UILoadMenu			(CUICustomMenu* owner, LPCSTR ini_name, LPCSTR start_sect, OnExecuteEvent exec=0, OnItemDrawEvent draw=0);

#endif //__XR_UICUSTOMMENU_H__