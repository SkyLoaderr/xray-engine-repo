#include "stdafx.h"
#pragma hdrstop

#include "GameFont.h"
#include "xr_ini.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MAX_CHARS	1024
CGameFont::CGameFont(LPCSTR shader, LPCSTR texture, u32 flags)
{
	uFlags						= flags;
	cShader						= xr_strdup(shader);
	cTexture					= xr_strdup(texture);
	pShader						= 0;
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
	for (int i=0; i<256; i++)	CharMap[i] = i;
	strings.reserve				(128);

	// check ini exist
	string256 fn,buf;
	strcpy(buf,texture); if (strext(buf)) *strext(buf)=0;
#ifdef M_BORLAND
	R_ASSERT(Engine.FS.Exist(fn,Engine.FS.m_GameTextures.m_Path,buf,".ini"));
#else
	R_ASSERT(Engine.FS.Exist(fn,Path.Textures,buf,".ini"));
#endif
	CInifile* ini				= CInifile::Create(fn);
	if (ini->SectionExists("symbol_coords")){
		for (int i=0; i<256; i++){
			sprintf					(buf,"%03d",i);
			Fvector v				= ini->ReadVECTOR("symbol_coords",buf);
			TCMap[i].set			(v.x,v.y,v[2]-v[0]);
		}
		fHeight						= ini->ReadFLOAT("symbol_coords","height");
		vInterval.set				(1.f,1.f);
	}else{
		if (ini->SectionExists("char widths")){
			fHeight					= ini->ReadFLOAT("char widths","height");
			const int cpl			= 16;
			for (int i=0; i<256; i++){
				sprintf				(buf,"%d",i);
				float w				= ini->ReadFLOAT("char widths",buf);
				TCMap[i].set		((i%cpl)*fHeight,(i/cpl)*fHeight,w);
			}
			vInterval.set			(1.f,1.f);
		}else{
			R_ASSERT(ini->SectionExists("font_size"));
			fHeight					= ini->ReadFLOAT("font_size","height");
			float width				= ini->ReadFLOAT("font_size","width");
			const int cpl			= ini->ReadINT	("font_size","cpl");
			vInterval.x				= ini->ReadFLOAT("font_size","interval");
			vInterval.y				= 1.f;
			for (int i=0; i<256; i++)
				TCMap[i].set			((i%cpl)*width,(i/cpl)*fHeight,width);
		}
	}
	if (!(uFlags&fsDeviceIndependent)) Size(fHeight);
	CInifile::Destroy			(ini);

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

	if (!(uFlags&fsValid)){
		CTexture* T		= Device.Shader.get_ActiveTexture(0);
		vTS.set			((int)T->get_Width(),(int)T->get_Height());
		vHalfPixel.set	(0.5f/float(vTS.x),0.5f/float(vTS.y));
		for (int i=0; i<256; i++){ 
			TCMap[i].x	/= float(vTS.x);
			TCMap[i].y	/= float(vTS.y);
			TCMap[i].z	/= float(vTS.x);
		}
		fTCHeight		= fHeight/float(vTS.y);
		uFlags			|= fsValid;
	}

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
				float	X	= float(iFloor(ConvertX(PS.x)));
				float	Y	= float(iFloor(ConvertY(PS.y)));
				float	S	= ConvertSize(PS.size);
				float	Y2	= Y+S;
				S			= S/fHeight*vTS.x;

				u32	clr,clr2; 
				clr			= PS.c; 
				if (uFlags&fsGradient){
					u32	_R	= RGBA_GETRED	(clr)/2;
					u32	_G	= RGBA_GETGREEN	(clr)/2;
					u32	_B	= RGBA_GETBLUE	(clr)/2;
					u32	_A	= RGBA_GETALPHA	(clr);
					clr2	= D3DCOLOR_RGBA	(_R,_G,_B,_A);
				}else{
					clr2	= clr;
				}

				float	tu,tv;
				for (int j=0; j<len; j++) {
					int c		= CharMap	[PS.string[j]];
					Fvector& l	= TCMap		[PS.string[j]];
					float scw	= S*l.z;
					if ((c>=0)&&!fis_zero(l.z)){
						tu		= l.x+vHalfPixel.x;
						tv		= l.y+vHalfPixel.y;
						v->set	(X,		Y2,	clr2,tu,		tv+fTCHeight);	v++;
						v->set	(X,		Y,	clr, tu,		tv);			v++;
						v->set	(X+scw,	Y2,	clr2,tu+l.z,	tv+fTCHeight);	v++;
						v->set	(X+scw,	Y,	clr, tu+l.z,	tv);			v++;
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
		rs.x-=SizeOf(rs.string)*.5f;
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
		rs.x-=SizeOf(rs.string)*.5f;
	} else {
		vsprintf(rs.string,fmt,p);
		VERIFY(strlen(rs.string)<127);
	}
	va_end(p);

	strings.push_back(rs);
	fCurrentY += fCurrentSize*vInterval.y;
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
		rs.x-=SizeOf(rs.string)*.5f;
	} else {
		vsprintf(rs.string,fmt,p);
		VERIFY(strlen(rs.string)<127);
	}
	va_end(p);

	strings.push_back(rs);
	fCurrentY -= fCurrentSize*vInterval.y;
}

float CGameFont::SizeOf(char *s)	
{ 
	int		len			= strlen(s);
	float	X			= 0;
	if (len) for (int j=0; j<len; j++) X+=TCMap[s[j]].z;
	X					= X*ConvertSize(fCurrentSize)/fHeight*vInterval.x*vTS.x;
	if (uFlags&fsDeviceIndependent)		return (2*X)/Device.dwWidth;
	else								return X;
}
