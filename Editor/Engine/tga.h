// file: targasaver.h

#ifndef _INCDEF_TARGASAVER_H_
#define _INCDEF_TARGASAVER_H_

#include "fs.h"

#pragma pack(push,1)
struct tgaImgSpecHeader{
	WORD tgaXOrigin;
	WORD tgaYOrigin;
	WORD tgaXSize;
	WORD tgaYSize;
	BYTE tgaDepth;
	BYTE tgaImgDesc;
};
struct tgaHeader{
	BYTE tgaIDL;
	BYTE tgaMapType;
	BYTE tgaImgType;
	BYTE tgaClrMapSpec[5];
	tgaImgSpecHeader tgaImgSpec;
};
#pragma pack(pop)


#define IMG_24B 0
#define IMG_32B 1

class ENGINE_API TGAdesc{
public:
	int format;
	int scanlenght;
	int width,height;
	void *data;
public:
	TGAdesc()	{ data = 0; };
	~TGAdesc()	{};

	VOID maketga( char *fname );
	VOID maketga( CFS_Base &fs );
};

#endif /*_INCDEF_TARGASAVER_H_*/

