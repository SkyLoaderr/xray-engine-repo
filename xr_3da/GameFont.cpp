#include "stdafx.h"
#pragma hdrstop

#include "GameFont.h"
#include "xr_ini.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MAX_CHARS	1024
CGameFont::CGameFont(LPCSTR shader, LPCSTR texture, int tsize, int iCPL, u32 flags)
{
	fScale						= 1.f;
	dwFlags						= flags;
	cShader						= xr_strdup(shader);
	cTexture					= xr_strdup(texture);
	iNumber						= iCPL;
	pShader						= 0;
	vHalfPixel.set				(0.5f/float(tsize),0.5f/float(tsize));
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
	vUVSize.set					(1.f/float(iNumber),1.f/float(iNumber));
	for (int i=0; i<256; i++){	CharMap[i] = i; WFMap[i] = 1.f;}
	strings.reserve				(128);

	Size						(float(tsize)/float(iCPL));
	// check ini exist
	string256 fn,buf;
	strcpy(buf,texture); if (strext(buf)) *strext(buf)=0;
#ifdef M_BORLAND
	if (!Engine.FS.Exist(fn,Engine.FS.m_GameTextures.m_Path,buf,	".ini")){
		ELog.Msg(mtError,"Can't find font ini file '%s'",fname);
#else
	if (Engine.FS.Exist(fn,Path.Textures,buf,".ini")){
#endif
		CInifile* ini			= CInifile::Create(fn);
		for (int i=0; i<256; i++)
			WFMap[i]			= ini->ReadFLOAT("Char Widths",itoa(i,buf,10))/fCurrentSize;
		CInifile::Destroy		(ini);
		vInterval.set			(1.f,1.f);
		dwFlags					|= fsVariableWidth;
	}else{
		if (dwFlags&fsDeviceIndependent)	vInterval.set	(0.65f,1.f);
		else								vInterval.set	(0.75f,1.f);
		dwFlags					&=~fsVariableWidth;
	}

	OnDeviceCreate				();
}

CGameFont::~CGameFont()
{
	xr_free						(cShader);
	xr_free						(cTexture);
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	Device.Shader.Delete		(pShader);
	OnDeviceDestroy				();
}

void CGameFont::OnDeviceCreate()
{
	REQ_CREATE				();
	pShader					= Device.Shader.Create		(cShader,cTexture);
	VS						= Device.Shader._CreateVS	(FVF::F_TL);
}
void CGameFont::OnDeviceDestroy()
{
	REQ_DESTROY				();
	Device.Shader.Delete	(pShader);
	Device.Shader._DeleteVS	(VS);
}

#ifndef RGBA_GETALPHA
#define RGBA_GETALPHA(rgb)      u32((rgb) >> 24)
#define RGBA_GETRED(rgb)        u32(((rgb) >> 16) & 0xff)
#define RGBA_GETGREEN(rgb)      u32(((rgb) >> 8) & 0xff)
#define RGBA_GETBLUE(rgb)       u32((rgb) & 0xff)
#endif

void CGameFont::OnRender()
{
	if (pShader) Device.Shader.set_Shader	(pShader);

	for (u32 i=0; i<strings.size(); ) 
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
		u32	vOffset;
		FVF::TL* v		= (FVF::TL*)Device.Streams.Vertex.Lock	(length*4,VS->dwStride,vOffset);
		FVF::TL* start	= v;
		
		// fill vertices
		u32 last=i+count;
		for (; i<last; i++) {
			String		&PS	= strings[i];
			int			len	= strlen(PS.string);
			if (len) {
				float	X	= float			(iFloor(ConvertX(PS.x)));
				float	Y	= float			(iFloor(ConvertY(PS.y)));
				float	S	= ConvertSize	(PS.size);
				float	Y2	= Y+S; 

				u32	clr,clr2; 
				clr			= PS.c; 
				if (dwFlags&fsGradient){
					u32	_R	= RGBA_GETRED	(clr)/2;
					u32	_G	= RGBA_GETGREEN	(clr)/2;
					u32	_B	= RGBA_GETBLUE	(clr)/2;
					u32	_A	= RGBA_GETALPHA	(clr);
					clr2		= D3DCOLOR_RGBA	(_R,_G,_B,_A);
				}else{
					clr2		= clr;
				}

				float	tu,tv;
				for (int j=0; j<len; j++) {
					int c		= CharMap	[PS.string[j]];
					float cw	= WFMap		[PS.string[j]];
					float scw	= S*cw;
					if (c>=0){
						tu		= (c%iNumber)*vUVSize.x+vHalfPixel.x;
						tv		= (c/iNumber)*vUVSize.y+vHalfPixel.y;
						v->set(X,		Y2,	clr2,tu,				tv+vUVSize.y);	v++;
						v->set(X,		Y,	clr, tu,				tv);			v++;
						v->set(X+scw,	Y2,	clr2,tu+vUVSize.x*cw,	tv+vUVSize.y);	v++;
						v->set(X+scw,	Y,	clr, tu+vUVSize.x*cw,	tv);			v++;
					}
					X			+=scw*vInterval.x;
				}
			}
		}

		// Unlock and draw
		u32 vCount = v-start;
		Device.Streams.Vertex.Unlock		(vCount,VS->dwStride);
		if (vCount) 
		{
			Device.Primitive.setVertices	(VS->dwHandle,VS->dwStride,Device.Streams.Vertex.Buffer());
			Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount,0,vCount/2);
		}
	}
	strings.clear();
}

void CGameFont::Add(float _x, float _y, char *s, u32 _c, float _size)
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

void __cdecl CGameFont::Out(float _x, float _y, char *fmt,...)
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

void __cdecl CGameFont::OutNext(char *fmt,...)
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

void __cdecl CGameFont::OutPrev(char *fmt,...)
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
