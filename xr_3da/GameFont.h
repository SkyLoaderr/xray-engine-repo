#ifndef _XR_GAMEFONT_H_
#define _XR_GAMEFONT_H_
#pragma once

class ENGINE_API CGameFont: public pureDeviceDestroy, public pureDeviceCreate
#ifndef M_BORLAND
	, public pureRender
#endif
{
	struct String {
		string128	string;
		float		x,y;
		float		size;
		u32		c;
	};
	LPSTR					cShader;
	LPSTR					cTexture;
protected:
	Fvector2				vHalfPixel;
	Fvector2				vUVSize;	

	u32					dwCurrentColor;
	float					fCurrentSize;
	float					fCurrentX, fCurrentY;
	Fvector2				vInterval;

	int						CharMap[256];
	float					WFMap[256];
	int						iNumber;
	vector<String>			strings;

	Shader*					pShader;
	CVS*					VS;

	u32					dwFlags;
	float					fScale;
public:
	enum{
		fsGradient			= (1<<0),
		fsDeviceIndependent	= (1<<1),
		fsVariableWidth		= (1<<2),
		fsForceDWORD		= (-1)
	};
protected:
	IC	float				ConvertX		(float x)	{return (dwFlags&fsDeviceIndependent)?Device._x2real(x):x*fScale;}
	IC	float				ConvertY		(float y)	{return (dwFlags&fsDeviceIndependent)?Device._y2real(y):y*fScale;}
	IC	float				ConvertSize		(float sz)	{return (dwFlags&fsDeviceIndependent)?sz*Device.dwWidth:sz*fScale;}
	IC	float				GetCurrentSize	()			{return (dwFlags&fsDeviceIndependent)?2*fCurrentSize:fCurrentSize*fScale;}
public:
							CGameFont		(LPCSTR shader, LPCSTR texture, int tsize, int iCPL=16, u32 flags=0);
							~CGameFont		();

	IC void					Color			(u32 C)	{dwCurrentColor=C;};
	IC void					Size			(float S)	{fCurrentSize=S;};
	IC void					Scale			(float S)	{fScale=S;};
	IC void					Interval		(float x, float y) {vInterval.set(x,y);};
	IC void					Add				(float _x, float _y, char *s, u32 _c=0xffffffff, float _size=0.01f);
	IC float				SizeOf			(char *s)	
	{ 
		if (dwFlags&fsVariableWidth){
			int		len			= strlen(s);
			float	X			= 0;
			if (len) {
				for (int j=0; j<len; j++) {
					float cw	= WFMap		[s[j]];
					X			+=cw;
				}
			}
			return X*fCurrentSize*vInterval.x; 
		}else{
			return fCurrentSize*vInterval.x*float(strlen(s)); 
		}
	}
	IC float				GetScale		()			{return fScale;}
	void					OutSet			(float x, float y)	{fCurrentX=x; fCurrentY=y;}
	void __cdecl            OutNext			(char *fmt, ...);
	void __cdecl            OutPrev			(char *fmt, ...);
	void __cdecl 			Out				(float _x, float _y, char *fmt, ...);
	IC void					OutSkip			(float val=1.f)		{fCurrentY += val*GetCurrentSize()*vInterval.y;}
	
	virtual void			OnRender		();
	virtual void			OnDeviceCreate	();
	virtual void			OnDeviceDestroy	();

	IC	void				Clear			()  { strings.clear(); }
};

#endif // _XR_GAMEFONT_H_
