#ifndef GameFontH
#define GameFontH
#pragma once

class ENGINE_API CGameFont
#ifndef M_BORLAND
	: public pureRender
#endif
{
public:
	enum EAligment{
		alLeft				= 0,
		alRight,
		alCenter
	};
private:
	struct String
	{
		string128	string;
		float		x,y;
		float		size;
		u32			c;
		EAligment	align;
	};
protected:
	Fvector2				vHalfPixel;
	Ivector2				vTS;

	EAligment				eCurrentAlignment;
	u32						dwCurrentColor;
	float					fCurrentSize;
	float					fCurrentX, fCurrentY;
	Fvector2				vInterval;

	int						CharMap	[256];
	Fvector 				TCMap	[256];
	float					fHeight;
	float					fTCHeight;
	xr_vector<String>		strings;

	ref_shader				pShader;
	ref_geom				pGeom;

	u32						uFlags;
public:
	enum
	{
		fsGradient			= (1<<0),
		fsDeviceIndependent	= (1<<1),
		fsValid 			= (1<<2),

		fsForceDWORD		= u32(-1)
	};

protected:
	IC int					GetCharRM		(u8 c)		{return CharMap[c];}
	IC const Fvector&		GetCharTC		(u8 c)		{return TCMap[c];}

	void					Initialize		(LPCSTR shader, LPCSTR texture, u32 flags);
public:
							CGameFont		(LPCSTR section, u32 flags=0);
							CGameFont		(LPCSTR shader, LPCSTR texture, u32 flags=0);
							~CGameFont		();

	IC float				ConvertSize		(float sz)	{return (uFlags&fsDeviceIndependent)?sz*Device.dwWidth:sz;}
	IC float				ConvertSize		(int sz)	{return (uFlags&fsDeviceIndependent)?(float)sz/Device.dwWidth:(float)sz;}
	IC void					SetColor		(u32 C)		{dwCurrentColor=C;};
	IC void					SetSize			(float S)	{fCurrentSize=S;};
	IC float				GetSize			()			{return fCurrentSize;};
	IC void					SetInterval		(float x, float y) {vInterval.set(x,y);};
	IC void					SetInterval		(const Fvector2& v) {vInterval.set(v);};
	IC void					SetAligment		(EAligment aligment){ eCurrentAlignment=aligment; }
	IC void					Add				(float _x, float _y, LPCSTR s, u32 _c=0xffffffff, float _size=0.01f);
	float					SizeOf			(char c, float size);
	IC float				SizeOf			(char c){return SizeOf(c,fCurrentSize);}
	float					SizeOf			(LPCSTR s, float size);
	IC float				SizeOf			(LPCSTR s){return SizeOf(s,fCurrentSize);}
	IC float				CurrentHeight	(){return fCurrentSize*vInterval.y*((uFlags&fsDeviceIndependent)?2.f:1.f);}
	void					OutSet			(float x, float y)	{fCurrentX=x; fCurrentY=y;}
	void __cdecl            OutNext			(LPCSTR fmt, ...);
	void __cdecl            OutPrev			(LPCSTR fmt, ...);
	void __cdecl 			Out				(float _x, float _y, LPCSTR fmt, ...);
	void					OutSkip			(float val=1.f);

	virtual void			OnRender		();

	IC	void				Clear			()  { strings.clear(); }
};

#endif // _XR_GAMEFONT_H_
