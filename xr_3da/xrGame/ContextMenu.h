#ifndef __XR_CONTEXTMENU_H__
#define __XR_CONTEXTMENU_H__
#pragma once

// refs
class CInifile;
class CFontBase;

class CContextMenu
{
public:
	struct MenuItem
	{
		char*					Name;
		EVENT					Event;
		char*					Param;
	};
	char*						Name;
	vector<MenuItem>			Items;
public:
				~CContextMenu	();
	void		Load			(CInifile* INI, LPCSTR S);
	void		Render			(CFontBase* F, DWORD cT, DWORD cI, float s);
	void		Select			(int num);
};
#endif // __XR_CONTEXTMENU_H__
