// tntQAVI.h

#ifndef __tntQAVI_H__
#define __tntQAVI_H__

#include <math.h>

//#include "tntTimer.h"
#include "vfw.h"
#include "mmsystem.h"

// replaced with standard AVIIF_KEYFRAME
//rr #define	AVIINDEX_ISKEYFRAME		0x10	// ключевой кадр

// reverse enginered AVI index v.1 format
/*struct AviIndex {

	DWORD	dwChunkType;	// chunk type, i.e. '##dc' - DIB compressed
	DWORD	dwFlags;		// key-frame etc.
	DWORD	dwOffset;		// sub-chunk offset from the begining of the LIST chunk
	DWORD	dwLenght;		// chunk lenght

};

typedef struct {
	FOURCC fccType;
	FOURCC fccHandler;
	DWORD  dwFlags;
	DWORD  dwPriority;
	DWORD  dwInitialFrames;
	DWORD  dwScale;
	DWORD  dwRate;
	DWORD  dwStart;
	DWORD  dwLength;
	DWORD  dwSuggestedBufferSize;
	DWORD  dwQuality;
	DWORD  dwSampleSize;
	RECT   rcFrame;
} AVIStreamHeader;
*/
typedef struct {
	FOURCC fccType;
	FOURCC fccHandler;
	DWORD  dwFlags;
	DWORD  dwPriority;
	DWORD  dwInitialFrames;
	DWORD  dwScale;
	DWORD  dwRate;
	DWORD  dwStart;
	DWORD  dwLength;
	DWORD  dwSuggestedBufferSize;
	DWORD  dwQuality;
	DWORD  dwSampleSize;
	struct {

		WORD	left;
		WORD	top;
		WORD	right;
		WORD	bottom;
	};
//	RECT   rcFrame;		- лажа в MSDN
} AVIStreamHeaderCustom;

class CAviPlayerCustom
{
protected:
	CAviPlayerCustom	*alpha;
protected:
	AVIINDEXENTRY		*m_pMovieIndex;
	BYTE				*m_pMovieData;
	HIC					m_aviIC;
	BYTE				*m_pDecompressedBuf;

	BITMAPINFOHEADER	m_biOutFormat;
	BITMAPINFOHEADER	m_biInFormat;

	float				m_fRate;		// стандартная скорость, fps
	float				m_fCurrentRate;	// текущая скорость, fps

	DWORD				m_dwFrameTotal;
	DWORD				m_dwFrameCurrent;


	DWORD				CalcFrame()
	{	return DWORD( floor(Device.TimerAsyncMM() * m_fCurrentRate / 1000.0f) ) % m_dwFrameTotal; }

	BOOL				DecompressFrame		( DWORD	dwFrameNum );
	VOID				PreRoll				( DWORD dwFrameNum );

public:
						CAviPlayerCustom		( );
						~CAviPlayerCustom		( );

	DWORD				m_dwWidth, m_dwHeight;

	VOID				GetSize				( DWORD *dwWidth, DWORD *dwHeight );
	
	BOOL				Load				( char *fname  );
	BOOL				GetFrame			( BYTE **pDest );

	BOOL				NeedUpdate			( ) { return CalcFrame( ) != m_dwFrameCurrent; }
	INT					SetSpeed			( INT nPercent );
};
#endif __tntQAVI_H__
