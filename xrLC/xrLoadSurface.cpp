#include "stdafx.h"
#include "freeimage.h"
#include "xr_func.h"

static	vector<string>	exts;
string	formats;

void	Format_Register	(string& F)
{
	if (0==F.length()) return;

	for (DWORD I=0; I<exts.size(); I++)
		if (exts[I]==F)	return;

	formats += F+",";
	exts.push_back(F);
	F = "";
}

void	Surface_FormatExt(FREE_IMAGE_FORMAT f)
{
	string	ext = string(FreeImage_GetFIFExtensionList(f))+",";
	string	cur;
	for		(DWORD i=0; i<ext.length(); i++)
	{
		if (ext[i]==',')	Format_Register(cur);
		else				cur += ext[i];
	}
	Format_Register	(cur);
}
void	Surface_Init()
{
	Msg("* ImageLibrary version: %s",FreeImage_GetVersion());

	Format_Register(string("tga"));
	Surface_FormatExt(FIF_BMP);
	Surface_FormatExt(FIF_ICO);
	Surface_FormatExt(FIF_JPEG);
	Surface_FormatExt(FIF_JNG);
	Surface_FormatExt(FIF_KOALA);
	Surface_FormatExt(FIF_LBM);
	Surface_FormatExt(FIF_MNG);
	Surface_FormatExt(FIF_PBM);
	Surface_FormatExt(FIF_PBMRAW);
	Surface_FormatExt(FIF_PCD);
	Surface_FormatExt(FIF_PCX);
	Surface_FormatExt(FIF_PGM);
	Surface_FormatExt(FIF_PGMRAW);
	Surface_FormatExt(FIF_PNG);
	Surface_FormatExt(FIF_PPM);
	Surface_FormatExt(FIF_PPMRAW);
	Surface_FormatExt(FIF_RAS);
	Surface_FormatExt(FIF_TARGA);
	Surface_FormatExt(FIF_TIFF);
	Surface_FormatExt(FIF_WBMP);
	Surface_FormatExt(FIF_PSD);
	Surface_FormatExt(FIF_IFF);

	Msg("* %d supported formats (%s)",exts.size(),formats.c_str());
}

BOOL	Surface_Detect(LPSTR F, LPSTR N)
{
	for (DWORD i=0; i<exts.size(); i++)
	{
		strconcat(F,"x:\\textures\\",N,".",exts[i].c_str());
		int h = _open(F,O_RDONLY|O_BINARY);
		if (h>0)	{
			_close(h);
			return TRUE;
		}
	}
	return FALSE;
}

DWORD*	Surface_Load(char* name, DWORD& w, DWORD& h)
{
	if (strchr(name,'.')) *(strchr(name,'.')) = 0;

	// detect format
	FILE_NAME		full;
	if (!Surface_Detect(full,name)) return NULL;

	// load
	FREE_IMAGE_FORMAT	fif		= FreeImage_GetFIFFromFilename(full);
	FIBITMAP*			map		= FreeImage_Load(fif,full);
	if (0==map)			return NULL;

	h					= FreeImage_GetHeight	(map);
	w					= FreeImage_GetWidth	(map);

	// check if already 32bpp
	if (32==FreeImage_GetBPP(map))	return LPDWORD(FreeImage_GetScanLine(map,0));

	// convert
	FIBITMAP*			map32	= FreeImage_ConvertTo32Bits(map);
	if (0==map32)		map32	= map;
	else				FreeImage_Free(map);

	// return
	return				LPDWORD(FreeImage_GetScanLine(map32,0));
}
