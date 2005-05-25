#pragma once

#include "../UICustomItem.h"

// Text & Font

class IUIFontControl{
public:
	typedef CGameFont::EAligment ETextAlignment;
	virtual ~IUIFontControl()											 {};

	virtual void			SetTextColor(u32 color)						= 0;
	virtual u32				GetTextColor()						const	= 0;
	virtual void			SetFont(CGameFont* pFont)					= 0;
	virtual CGameFont*		GetFont()							const	= 0;
	virtual void			SetTextAlignment(ETextAlignment alignment)	= 0;
	virtual ETextAlignment	GetTextAlignment()					const	= 0;
};

class IUITextControl : public IUIFontControl{
public:
	virtual ~IUITextControl()											 {};
	virtual void SetText(const char* text)								= 0;
	virtual const char* GetText()								const	= 0;
};


// Texture controls
class IUISimpleTextureControl{
public:
	virtual ~IUISimpleTextureControl() {}
	virtual void		CreateShader(const char* tex, const char* sh = "hud\\default")	= 0;
	virtual void		SetShader(const ref_shader& sh)									= 0;
//	virtual ref_shader&	GetShader()														= 0;
	virtual void		SetTextureColor(u32 color)										= 0;
	virtual u32			GetTextureColor()										const	= 0;
	virtual void		SetOriginalRect(const Irect& r)									= 0;
	virtual void		SetOriginalRectEx(const Irect& r)								= 0;
};

class IUIMultiTextureOwner{
public:
	virtual ~IUIMultiTextureOwner() {}
	virtual void		InitTexture(const char* texture)								= 0;
	virtual bool		GetTextureAvailability()										= 0;
	virtual void		SetTextureVisible(bool vis)										= 0;
	virtual bool		GetTextureVisible()												= 0;
};

class CUIMultiTextureOwner : public IUIMultiTextureOwner{
public:
	virtual bool		GetTextureAvailability()	{return m_bTextureAvailable;}
	virtual void		SetTextureVisible(bool vis)	{m_bTextureVisible = true;}
	virtual bool		GetTextureVisible()			{return m_bTextureVisible;}
protected:
	bool m_bTextureAvailable;
	bool m_bTextureVisible;
};

class IUISingleTextureOwner : public CUIMultiTextureOwner, public IUISimpleTextureControl{
public:
	virtual void		SetStretchTexture(bool stretch)									= 0;
	virtual bool		GetStretchTexture()												= 0;	
};

class CUISingleTextureOwner : public IUISingleTextureOwner{
public:
	virtual void		SetStretchTexture(bool stretch)	{m_bStretchTexture = stretch;}
	virtual bool		GetStretchTexture()				{return m_bStretchTexture;}
protected:
	bool m_bStretchTexture;
};

// Window
enum EWindowAlignment{
	waNone		=0,
	waLeft		=1,
	waRight		=2,
	waTop		=4,
	waBottom	=8,
	waCenter	=16
};

class IUISimpleWindow{
public:
	virtual ~IUISimpleWindow()						{};
    
	virtual void		Init(int x, int y, int width, int height)		= 0;
	virtual void		Draw()											= 0;
	virtual void		Draw(int x, int y)								= 0;
	virtual void		Update()										= 0;
	virtual void		SetWndPos(const Ivector2& pos)					= 0;
	virtual void		SetWndPos(int x, int y)							= 0;
	virtual Ivector2	GetWndPos()								const	= 0;
	virtual void		SetWndSize(const Ivector2& size)				= 0;
	virtual Ivector2	GetWndSize()							const	= 0;
	virtual void		SetWndRect(const Irect& rect)					= 0;
	virtual Irect		GetWndRect()							const	= 0;
	virtual void		SetHeight(int height)							= 0;
	virtual int			GetHeight()								const	= 0;
	virtual void		SetWidth(int width)								= 0;
	virtual int			GetWidth()								const	= 0;
	virtual void		SetVisible(bool vis)							= 0;
	virtual bool		GetVisible()							const	= 0;
	virtual void		SetAlignment(EWindowAlignment al)				= 0;
};

class CUISimpleWindow : public IUISimpleWindow {
public:
							CUISimpleWindow()							{m_alignment=waNone;}
	virtual void			Init(int x, int y, int width, int height)	{m_wndPos.set(x,y);m_wndSize.set(width, height);}
	virtual void			SetWndPos(const Ivector2& pos)				{m_wndPos = pos;}
	virtual void			SetWndPos(int x, int y)						{m_wndPos.set(x,y);}
	virtual Ivector2		GetWndPos()							const	{return m_wndPos;}
	virtual void			SetWndSize(const Ivector2& size)			{m_wndSize = size;}
	virtual Ivector2		GetWndSize()						const	{return m_wndSize;}
	virtual void			SetHeight(int height)						{m_wndSize.y = height;}
	virtual int				GetHeight()							const	{return m_wndSize.y;}
	virtual void			SetWidth(int width)							{m_wndSize.x = width;}
	virtual int				GetWidth()							const	{return m_wndSize.x;}
	virtual void			SetVisible(bool vis)						{m_bShowMe = vis;}
	virtual bool			GetVisible()						const	{return m_bShowMe;}
	virtual void			SetAlignment(EWindowAlignment al)			{m_alignment = al;};
	virtual void			SetWndRect(const Irect& rect){
		m_wndPos.x = rect.x1;
		m_wndPos.y = rect.y1;
		m_wndSize.x = rect.x2 - rect.x1;
		m_wndSize.y = rect.y2 - rect.y1;
	}
/*	virtual Irect			GetWndRect()						const{
		Irect r;
		r.x1 = m_wndPos.x;
		r.y1 = m_wndPos.y;
		r.x2 = m_wndPos.x + m_wndSize.x;
		r.y2 = m_wndPos.y + m_wndSize.y;
		return r;
	}
*/
	virtual Irect GetWndRect()									const
	{
		switch (m_alignment){
			case waNone:
				return Irect().set(m_wndPos.x,m_wndPos.y,m_wndPos.x+m_wndSize.x,m_wndPos.y+m_wndSize.y);
				break;
			case waCenter:{
					Irect res;
					int half_w = iFloor(float(m_wndSize.x)/2.0f);
					int half_h = iFloor(float(m_wndSize.y)/2.0f);
					res.set(m_wndPos.x - half_w,
							m_wndPos.y - half_h,
							m_wndPos.x + half_w,
							m_wndPos.y + half_h);
					return res;
				}break;
			default:
				NODEFAULT;
		};
		return Irect().null();
	}

protected:    
	bool					m_bShowMe;
	Ivector2				m_wndPos;
	Ivector2				m_wndSize;
	EWindowAlignment		m_alignment;
};
