#if !defined(AFX_XR_CUSTOMFONT_H__6CFEF2C0_D4F5_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_CUSTOMFONT_H__6CFEF2C0_D4F5_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

class ENGINE_API CFontBase  : public pureRender
{
	struct String {
		char	string[128];
		float	x,y;
		float	size;
		DWORD	c;
	};
protected:
	DWORD					dwCurrentColor;
	float					fCurrentSize;
	float					fCurrentX, fCurrentY;
	Fvector2				TextureSize;
	Fvector2				vInterval;

	int						CharMap[256];
	int						iNumber;
	vector<String>			strings;
	Shader*					pShader;
	
	BOOL					bGradient;
protected:
	IC virtual	float		GetRealX(float x)		{return Device._x2real(x);}
	IC virtual	float		GetRealY(float y)		{return Device._y2real(y);}
	IC virtual	float		GetRealWidth(float sz)	{return sz*Device.dwWidth;}
	IC virtual	float		GetRealHeight(float sz)	{return sz*Device.dwWidth;}
	IC virtual	float		GetCurrentSize()		{return 2*fCurrentSize;}

public:
	CFontBase				();
	~CFontBase				();

	CVertexStream*			Stream;

	IC void					Color	(DWORD C) {dwCurrentColor=C;};
	IC void					Size	(float S) {fCurrentSize=S;};
	IC void					Interval(float x, float y) {vInterval.set(x,y);};
	IC void					Add		(float _x, float _y, char *s, DWORD _c=0xffffffff, float _size=0.01f);
	IC float				SizeOf	(char *s) { return fCurrentSize*.66f*float(strlen(s)); }
	void					OutSet	(float x, float y) { fCurrentX=x; fCurrentY=y; }
	void __cdecl            OutNext	(char *fmt, ...);
	void __cdecl            OutPrev	(char *fmt, ...);
	void					OutSkip	()	{ 	fCurrentY += fCurrentSize*2.f; }
	void __cdecl 			Out		(float _x, float _y, char *fmt, ...);

	virtual void			OnRender(void);

	IC	void				Clear	()  { strings.clear(); }
};

#endif // !defined(AFX_XR_CUSTOMFONT_H__6CFEF2C0_D4F5_11D3_B4E3_4854E82A090D__INCLUDED_)
