#ifndef __XR_UISTATICITEM_H__
#define __XR_UISTATICITEM_H__
#pragma once

#include "uicustomitem.h"

class CUIStaticItem: public CUICustomItem
{
	Shader*			hShader;
	CVS*			hVS;	

	Ivector2		iPos;
	u32				dwColor;
	int				iTileX;
	int				iTileY;
	int				iRemX;
	int				iRemY;
protected:
	typedef CUICustomItem inherited;
public:
					CUIStaticItem	();
	virtual			~CUIStaticItem	();
	void			Init			(LPCSTR tex, LPCSTR sh, int left, int top, u32 align);
	void			CreateShader	(LPCSTR tex, LPCSTR sh);
	void			Render			(Shader* sh=0);
	void			Render			(float angle, Shader* sh=0);
	
	IC void			SetTile			(int tile_x, int tile_y, int rem_x, int rem_y){iTileX=tile_x;iTileY=tile_y;iRemX=rem_x;iRemY=rem_y;}
	IC void			SetPos			(int left, int top)			{iPos.set(left,top);}
	IC void			SetPosX			(int left)					{iPos.x = left;}
	IC void			SetPosY			(int top)					{iPos.y = top;}
	IC void			SetColor		(u32 clr)					{dwColor= clr;}
	IC void			SetColor		(Fcolor clr)				{dwColor= clr.get();}
	IC Shader*		GetShader		()							{return hShader;}
};

#endif //__XR_UISTATICITEM_H__
