#ifndef __XR_AVI_H__
#define __XR_AVI_H__
#include "vfw.h"

class ENGINE_API		CAviPlayerCustom
{
public:
	DWORD				dwBufferSize;
	void				*pBuffer;

	PAVISTREAM			aviStream;
	HIC					aviIC;
	BITMAPINFOHEADER	aviOutFormat;
	BITMAPINFOHEADER	aviInFormat;

	float				fRate;
	DWORD				dwFrameTotal;
	DWORD				dwFrameCurrent;

	DWORD				dwWidth,dwHeight;

	DWORD				CalcFrame()
	{	return iFloor(Device.fTimeGlobal*fRate)%dwFrameTotal; }
public:
						CAviPlayerCustom	( );
						~CAviPlayerCustom	( );

	BOOL				Load				( char* fn     );
	BOOL				DecompressFrame		( DWORD* pDest );

	IC BOOL				NeedUpdate			() { return CalcFrame()!=dwFrameCurrent; }
};
#endif //__XR_AVI_H__
