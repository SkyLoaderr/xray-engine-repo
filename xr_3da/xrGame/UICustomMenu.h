#ifndef __XR_UICUSTOMMENU_H__
#define __XR_UICUSTOMMENU_H__
#pragma once

class					CCustomMenuItem;
DEFINE_VECTOR			(CCustomMenuItem*,MIVec,MIIt);
typedef void 			(*OnExecuteEvent)	(CCustomMenuItem* sender);
typedef void			(*OnItemDrawEvent)	(CGameFont* F, int num, int col);

class CCustomMenuItem{
	CCustomMenuItem*	m_Parent;
	MIVec				items;
	OnExecuteEvent		OnExecute;
	OnItemDrawEvent		OnItemDraw;
	LPSTR				OnExecuteCC;
public:
	LPSTR				caption;
	LPSTR				value;
	int					tag;
public:
	CCustomMenuItem		(CCustomMenuItem* parent, LPCSTR text, LPCSTR val, LPCSTR execCC=0, OnExecuteEvent exec=0, OnItemDrawEvent draw=0)
	{
		m_Parent		= parent;
		caption			= text?xr_strdup(text):0;
		value			= val?xr_strdup(val):0;
		tag				= 0;
		OnExecute		= exec;
		OnItemDraw		= draw;
		OnExecuteCC		= execCC?xr_strdup(execCC):0;
	}
	~CCustomMenuItem	()
	{
		_FREE			(OnExecuteCC);
		_FREE			(value);
		_FREE			(caption);
		for (MIIt it=items.begin(); it!=items.end(); it++)
			_DELETE		(*it);
	}
	void				AppendItem			(CCustomMenuItem* I)
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
			case 1: if (!HasChildren())	F->OutNext	("%s",value);			break;
			default: THROW;
			}
		}
	}
	IC BOOL				HasChildren			()			{return !items.empty();}
	IC CCustomMenuItem*	Parent				()			{return m_Parent;}
	IC MIIt				FirstItem			()			{return items.begin();}
	IC MIIt				LastItem			()			{return items.end();}
	IC DWORD			ItemCount			()			{return items.size();}
	IC CCustomMenuItem*	GetItem				(int id)	
	{
		id--;
		if (-1==id) return m_Parent;
		if (id<(int)(items.size())) return items[id];
		return 0;
	}
	void				Execute				();
};
CCustomMenuItem*		UIParseMenu			(CInifile* ini, CCustomMenuItem* root, LPCSTR sect, OnExecuteEvent exec=0, OnItemDrawEvent draw=0);
CCustomMenuItem*		UILoadMenu			(LPCSTR ini_name, OnExecuteEvent exec=0, OnItemDrawEvent draw=0);
CCustomMenuItem*		UIFindMenuItem		(CCustomMenuItem* root, LPCSTR name);

#endif //__XR_UICUSTOMMENU_H__