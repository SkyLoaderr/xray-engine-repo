#ifndef __XR_UIDYNAMICITEM_H__
#define __XR_UIDYNAMICITEM_H__
#pragma once

#include "uicustomitem.h"

struct SDynamicItemData{
	Ivector2		pos;
	DWORD			color;
};

DEFINE_VECTOR(SDynamicItemData,DIDVec,DIDIt);

class CUIDynamicItem: public CUICustomItem
{
	Shader*			hShader;
	CVS*			hVS;	
		
	DIDVec			data;
	DWORD			item_cnt;
protected:
	typedef CUICustomItem inherited;
public:
					CUIDynamicItem	();
	virtual			~CUIDynamicItem	();
	void			Init			(LPCSTR tex, LPCSTR sh, int width, int tx_height);
	void			Out				(int left, int top, DWORD color, DWORD align);
	void			Clear			(){item_cnt=0;}
	void			Render			();
	void			Render			(float angle);
};

#endif //__XR_UIDYNAMICITEM_H__
