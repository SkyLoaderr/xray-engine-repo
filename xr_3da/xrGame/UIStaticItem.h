#ifndef __XR_UISTATICITEM_H__
#define __XR_UISTATICITEM_H__
#pragma once

#include "uicustomitem.h"

class CUIStaticItem: public CUICustomItem
{
	Shader*			hShader;
	CVS*			hVS;	

	Fvector2		vPos;
	DWORD			dwColor;
	int				iTileX;
	int				iTileY;
	int				iRemX;
	int				iRemY;
protected:
	typedef CUICustomItem inherited;
public:
					CUIStaticItem	();
	virtual			~CUIStaticItem	();
	void			Init			(LPCSTR tex, LPCSTR sh, float left, float top, float tx_width, float tx_height, DWORD align);
	void			Init			(float left, float top, float tx_width, float tx_height, DWORD align);
	void			SetTile			(int tile_x, int tile_y, int rem_x, int rem_y){iTileX=tile_x;iTileY=tile_y;iRemX=rem_x;iRemY=rem_y;}
	void			SetPos			(float left, float top){vPos.set(left,top);}
	void			SetPosX			(float left){vPos.x = left;}
	void			SetPosY			(float top)	{vPos.y = top;}
	void			SetColor		(DWORD clr){dwColor=clr;}
	void			SetColor		(Fcolor clr){dwColor=clr.get();}
	void			Render			(Shader* sh=0);
	void			Render			(float angle, Shader* sh=0);
};

#endif //__XR_UISTATICITEM_H__
