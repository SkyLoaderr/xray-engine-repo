#ifndef __XR_UIGAMEDM_H__
#define __XR_UIGAMEDM_H__
#pragma once

#include "UIGameCustom.h"


#include "ui\\UIDialogWnd.h"
#include "ui\\UIInventoryWnd.h"


// refs 
class CUIDMFragList;
class CUIDMPlayerList;

class CUIGameDM: public CUIGameCustom
{
private:
	typedef CUIGameCustom inherited;
protected:
	CUIDMFragList*		pFragList;
	CUIDMPlayerList*	pPlayerList;
public:
						CUIGameDM			(CUI* parent);
	virtual 			~CUIGameDM			();
	virtual void		Render				();
	virtual void		OnFrame				();

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);

	/////////
	CUIInventoryWnd		InventoryMenu;
};
#endif // __XR_UIGAMEDM_H__
