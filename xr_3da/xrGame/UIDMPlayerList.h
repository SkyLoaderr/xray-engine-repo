#ifndef __XR_UIDMPLAYERLIST_H__
#define __XR_UIDMPLAYERLIST_H__
#pragma once

//#include "uiListBox.h"
#include "UIDMFragList.h"

class CUIDMPlayerList:public CUIDMFragList
{
	typedef CUIDMFragList inherited;
public:
//	void			Init			();
//	virtual void	OnFrame			();
	virtual void Update();
};

#endif //__XR_UIDMPLAYERLIST_H__
