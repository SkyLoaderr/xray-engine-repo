#ifndef __XR_UISTATICITEM_H__
#define __XR_UISTATICITEM_H__
#pragma once

#include "uicustomitem.h"

class CUIStaticItem: public CUICustomItem
{
	ref_shader		hShader;
	ref_geom		hGeom;	

	Ivector2		iPos;
	u32				dwColor;
	int				iTileX;
	int				iTileY;
	int				iRemX;
	int				iRemY;
	bool			bReverseRemX, bReverseRemY;

protected:
	typedef CUICustomItem inherited;
public:
					CUIStaticItem	();
	virtual			~CUIStaticItem	();
	void			Init			(LPCSTR tex, LPCSTR sh, int left, int top, u32 align);
	void			CreateShader	(LPCSTR tex, LPCSTR sh);
	void			SetShader		(const ref_shader& sh);
	
	void			Render			(const ref_shader& sh=ref_shader(0));
	void			Render			(float angle, const ref_shader& sh=ref_shader(0));
	
	void			Render			(float x1, float y1, float x2, float y2, 
									 float x3, float y3, float x4, float y4, 
										const ref_shader& sh=ref_shader(0));
	
	//вывод изображения из OriginalRect на текстуре в заданную область экрана 
	void			Render			(int x1, int y1, int x2, int y2, 
										const ref_shader& sh=ref_shader(0));


	IC void			SetTile			(int tile_x, int tile_y, int rem_x, int rem_y){iTileX=tile_x;iTileY=tile_y;iRemX=rem_x;iRemY=rem_y;}
	IC void			SetPos			(int left, int top)			{iPos.set(left,top);}
	IC void			SetPosX			(int left)					{iPos.x = left;}
	IC void			SetPosY			(int top)					{iPos.y = top;}

	IC int			GetPosX			()							{return iPos.x;}
	IC int			GetPosY			()							{return iPos.y;}

	IC void			SetColor		(u32 clr)					{dwColor= clr;}
	IC void			SetColor		(Fcolor clr)				{dwColor= clr.get();}
	IC u32			GetColor		() const					{return dwColor;}
	IC u32&			GetColorRef		()							{return dwColor;}
	IC ref_shader&	GetShader		()							{return hShader;}
	IC void			SetReverseRem	(bool valueX, bool valueY)	{bReverseRemX = valueX; bReverseRemY = valueY;}
};

#endif //__XR_UISTATICITEM_H__
