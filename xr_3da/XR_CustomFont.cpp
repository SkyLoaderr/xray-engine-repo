#include "stdafx.h"
#pragma hdrstop

#include "XR_CustomFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MAX_CHARS	1024
CFontBase::CFontBase()
{
	Stream			= Device.Streams.Create(FVF::F_TL,MAX_CHARS*4);
	iNumber			= 0;
	pShader			= 0;
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);

	strings.reserve	(128);
}

CFontBase::~CFontBase()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	OnDeviceDestroy	();
}

void CFontBase::OnDeviceCreate()
{
	REQ_CREATE		();
	OnInitialize	();
}
void CFontBase::OnDeviceDestroy()
{
	REQ_DESTROY();
	Device.Shader.Delete	(pShader);
}

#ifndef RGBA_GETALPHA
#define RGBA_GETALPHA(rgb)      DWORD((rgb) >> 24)
#define RGBA_GETRED(rgb)        DWORD(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      DWORD(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       DWORD((rgb) & 0xff)
#endif

void CFontBase::OnRender()
{
	Log("CFontBase::OnRender - begin");

	if (pShader) Device.Shader.set_Shader	(pShader);
	Fvector2 UVSize;	UVSize.set(1.f/float(iNumber),1.f/float(iNumber));
	Fvector2 HalfPixel;	HalfPixel.set(.5f/TextureSize.x,.5f/TextureSize.y);

	for (DWORD i=0; i<strings.size(); ) 
	{
		// calculate first-fit
		int		count	=	1;
		int		length	=	strlen(strings[i].string);
		while	((i+count)<strings.size()) {
			int	L	=	strlen(strings[i+count].string);
			if ((L+length)<MAX_CHARS)	
			{
				count	++;
				length	+=	L;
			}
			else		break;
		}

		// lock AGP memory
		DWORD	vOffset;
		FVF::TL* v		= (FVF::TL*)Stream->Lock(length*4,vOffset);
		FVF::TL* start	= v;
		Msg("CFontBase::LOCK %d, %x",length,v);
		
		// fill vertices
		DWORD last=i+count;
		for (; i<last; i++) {
			Msg("CFontBase:: %d / %d",i,strings.size());
			String		&PS	= strings[i];
			int			len	= strlen(PS.string);
			Msg("1");
			if (len) {
				Msg("a");
				float	X	= GetRealX(PS.x);
				float	Y	= GetRealY(PS.y);
				float	W	= GetRealWidth(PS.size);
				float	H	= GetRealHeight(PS.size);
				float	Y2	= Y+H;

				DWORD	clr,clr2; 
				clr			= PS.c; 
				if (bGradient){
					DWORD	_R	= RGBA_GETRED	(clr)/2;
					DWORD	_G	= RGBA_GETGREEN	(clr)/2;
					DWORD	_B	= RGBA_GETBLUE	(clr)/2;
					DWORD	_A	= RGBA_GETALPHA	(clr);
					clr2		= D3DCOLOR_RGBA	(_R,_G,_B,_A);
				}else{
					clr2		= clr;
				}

				Msg("b");
				float	tu,tv;
				for (int j=0; j<len; j++) {
					Msg("loop: %d, %d",j,len);
					int c = CharMap[PS.string[j]];
					Msg("*1");
					if (c>=0) {
						Msg("*2, %d",iNumber);
						tu = (c%iNumber)*UVSize.x+HalfPixel.x;
						Msg("*3");
						tv = (c/iNumber)*UVSize.y+HalfPixel.y;
						Msg("*4");
						v->set(X,	Y2,	clr2,tu,tv+UVSize.y);			v++;
						Msg("*5");
						v->set(X,	Y,	clr, tu,tv);					v++;
						Msg("*6");
						v->set(X+W,	Y2,	clr2,tu+UVSize.x,tv+UVSize.y);	v++;
						Msg("*7");
						v->set(X+W,	Y,	clr, tu+UVSize.x,tv);			v++;
						Msg("*8");
					}
					X+=W*vInterval.x;
				}
			}
			Msg("2");
		}

		// Unlock and draw
		DWORD vCount = v-start;
		Stream->Unlock			(vCount);
		Device.Primitive.Draw	(Stream,vCount,vCount/2,vOffset,Device.Streams_QuadIB);
	}
	strings.clear();

	Log("CFontBase::OnRender - end");
}

void CFontBase::Add(float _x, float _y, char *s, DWORD _c, float _size)
{
	Log("CFontBase::Add");
	VERIFY(strlen(s)<127);
	String rs;
	rs.x=_x;
	rs.y=_y;
	rs.c=_c;
	rs.size=_size;
	strings.push_back(rs);
	strcpy((char *) &(strings[strings.size()-1].string),s);
}

void __cdecl CFontBase::Out(float _x, float _y, char *fmt,...)
{
	Log("CFontBase::Out");

	String rs;
	rs.x=_x;
	rs.y=_y;
	rs.c=dwCurrentColor;
	rs.size=fCurrentSize;

	va_list p;
	va_start(p,fmt);
	if (fmt[0]=='~') {
		vsprintf(rs.string,&(fmt[1]),p);
		VERIFY(strlen(rs.string)<127);
		rs.x-=SizeOf(rs.string);
	} else {
		vsprintf(rs.string,fmt,p);
		VERIFY(strlen(rs.string)<127);
	}
	va_end(p);

	strings.push_back(rs);
}

void __cdecl CFontBase::OutNext(char *fmt,...)
{
	Log("CFontBase::OutNext");

	String rs;
	rs.x=fCurrentX;
	rs.y=fCurrentY;
	rs.c=dwCurrentColor;
	rs.size=fCurrentSize;

	va_list p;
	va_start(p,fmt);
	if (fmt[0]=='~') {
		vsprintf(rs.string,&(fmt[1]),p);
		VERIFY(strlen(rs.string)<127);
		rs.x-=SizeOf(rs.string);
	} else {
		vsprintf(rs.string,fmt,p);
		VERIFY(strlen(rs.string)<127);
	}
	va_end(p);

	strings.push_back(rs);
	fCurrentY += GetCurrentSize()*vInterval.y;
}

void __cdecl CFontBase::OutPrev(char *fmt,...)
{
	Log("CFontBase::OutPrev");
	
	String rs;
	rs.x=fCurrentX;
	rs.y=fCurrentY;
	rs.c=dwCurrentColor;
	rs.size=fCurrentSize;

	va_list p;
	va_start(p,fmt);
	if (fmt[0]=='~') {
		vsprintf(rs.string,&(fmt[1]),p);
		VERIFY(strlen(rs.string)<127);
		rs.x-=SizeOf(rs.string);
	} else {
		vsprintf(rs.string,fmt,p);
		VERIFY(strlen(rs.string)<127);
	}
	va_end(p);

	strings.push_back(rs);
	fCurrentY -= GetCurrentSize()*vInterval.y;
}
