#ifndef __XR_UISTATICITEM_H__
#define __XR_UISTATICITEM_H__
#pragma once

#include "ui/uiabstract.h"
#include "uicustomitem.h"

class CUIStaticItem: public IUISimpleTextureControl, public CUICustomItem
{
	ref_shader		hShader;
	ref_geom		hGeom_list;	
	ref_geom		hGeom_fan;	

	Fvector2		iPos;
	u32				dwColor;
	int				iTileX;
	int				iTileY;
	float			iRemX;
	float			iRemY;
protected:
	typedef CUICustomItem inherited;
public:
	using CUICustomItem::SetOriginalRect;

#ifdef DEBUG
//	string_path		dbg_tex_name;
#endif
					CUIStaticItem	();
	virtual			~CUIStaticItem	();

//GetShader()		
	//IUISimpleTextureControl
	virtual void	CreateShader	(const char* tex, const char* sh = "hud\\default");
	virtual void	SetShader		(const ref_shader& sh);
	virtual void	SetTextureColor	(u32 color)											{SetColor(color);}
	virtual u32		GetTextureColor	()											const	{return GetColor();}
	virtual	void	SetOriginalRect	(const Frect& r)									{iOriginalRect = r; uFlags|=flValidOriginalRect;}
	virtual void	SetOriginalRectEx(const Frect& r)									{iOriginalRect = r; uFlags|=flValidOriginalRect; SetRect(0,0,r.width(),r.height());}


	void			Init			(LPCSTR tex, LPCSTR sh, float left, float top, u32 align);
	
	
	
	void			Render			(const ref_shader& sh=ref_shader(0));
	void			Render			(float angle, const ref_shader& sh=ref_shader(0));

	IC void			SetTile			(int tile_x, int tile_y, float rem_x, float rem_y){iTileX=tile_x;iTileY=tile_y;iRemX=rem_x;iRemY=rem_y;}
	IC void			SetPos			(float left, float top)			{iPos.set(left,top);}
	IC void			SetPosX			(float left)					{iPos.x = left;}
	IC void			SetPosY			(float top)						{iPos.y = top;}

	IC float		GetPosX			()							{return iPos.x;}
	IC float		GetPosY			()							{return iPos.y;}

	IC void			SetColor		(u32 clr)					{dwColor= clr;}
	IC void			SetColor		(Fcolor clr)				{dwColor= clr.get();}
	IC u32			GetColor		() const					{return dwColor;}
	IC u32&			GetColorRef		()							{return dwColor;}
	IC ref_shader&	GetShader		()							{return hShader;}
};

extern ENGINE_API BOOL g_bRendering; 

#endif //__XR_UISTATICITEM_H__

