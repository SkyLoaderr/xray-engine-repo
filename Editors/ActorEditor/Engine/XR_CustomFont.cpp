#include "stdafx.h"
#pragma hdrstop

#include "XR_CustomFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MAX_CHARS	1024
CFontBase::CFontBase()
{
	iNumber						= 0;
	pShader						= 0;
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);

	strings.reserve				(128);
}

CFontBase::~CFontBase()
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	Device.Shader.Delete		(pShader);
	OnDeviceDestroy				();
}

void CFontBase::OnDeviceCreate()
{
	REQ_CREATE				();
	OnInitialize			();
	VS						= Device.Shader._CreateVS	(FVF::F_TL);
}
void CFontBase::OnDeviceDestroy()
{
	REQ_DESTROY				();
	Device.Shader.Delete	(pShader);
	Device.Shader._DeleteVS	(VS);
}

#ifndef RGBA_GETALPHA
#define RGBA_GETALPHA(rgb)      DWORD((rgb) >> 24)
#define RGBA_GETRED(rgb)        DWORD(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      DWORD(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       DWORD((rgb) & 0xff)
#endif

void CFontBase::OnRender()
{
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
		FVF::TL* v		= (FVF::TL*)Device.Streams.Vertex.Lock	(length*4,VS->dwStride,vOffset);
		FVF::TL* start	= v;
		
		// fill vertices
		DWORD last=i+count;
		for (; i<last; i++) {
			String		&PS	= strings[i];
			int			len	= strlen(PS.string);
			if (len) {
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

				float	tu,tv;
				for (int j=0; j<len; j++) {
					int c = CharMap[PS.string[j]];
					if (c>=0) {
						tu = (c%iNumber)*UVSize.x+HalfPixel.x;
						tv = (c/iNumber)*UVSize.y+HalfPixel.y;
						v->set(X,	Y2,	clr2,tu,tv+UVSize.y);			v++;
						v->set(X,	Y,	clr, tu,tv);					v++;
						v->set(X+W,	Y2,	clr2,tu+UVSize.x,tv+UVSize.y);	v++;
						v->set(X+W,	Y,	clr, tu+UVSize.x,tv);			v++;
					}
					X+=W*vInterval.x;
				}
			}
		}

		// Unlock and draw
		DWORD vCount = v-start;
		Device.Streams.Vertex.Unlock	(vCount,VS->dwStride);
		if (vCount) 
		{
			Device.Primitive.setVertices	(VS->dwHandle,VS->dwStride,Device.Streams.Vertex.getBuffer());
			Device.Primitive.setIndices		(vOffset,Device.Streams_QuadIB);
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount,0,vCount/2);
			UPDATEC							(vCount,vCount/2,1);
		}
	}
	strings.clear();
}

void CFontBase::Add(float _x, float _y, char *s, DWORD _c, float _size)
{
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
