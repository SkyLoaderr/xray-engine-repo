///////////////////////////////////////////////////////////////
//	Module 		: xr_ai.h
//	Created 	: 20.03.2002
//  Modified 	: 21.03.2002
//	Author		: Dmitriy Iassenev
//	Description : Artificial Intelligence for FPS X-Ray
///////////////////////////////////////////////////////////////

#ifndef __XR_TYPES__
#define __XR_TYPES__

///////////////////////////////////////////////////////////////
// included headers
///////////////////////////////////////////////////////////////

#include <windows.h>

///////////////////////////////////////////////////////////////
// macros definitions
///////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#define __ASSERT(bExpression)\
	{\
		if (!(bExpression))\
			printf("Assertion failed in module %s line %d\n",__FILE__,__LINE__);\
	}
#else
	#define __ASSERT(bExpression) ;
#endif

///////////////////////////////////////////////////////////////
// type declarations
///////////////////////////////////////////////////////////////

typedef unsigned int uint;
typedef unsigned char uchar;

typedef struct tagFbox {
	float x1, y1, z1;
	float x2, y2, z2;
} Fbox;

typedef struct taghdrNodes {
	DWORD	version;
	DWORD	count;
	float	size;
	float	size_y;
	Fbox	aabb;
} hdrNodes;

#pragma pack(1)

typedef BYTE		NodeLink[3];
typedef short		NodePosition[3];

typedef struct tagNodeCompressed {
	WORD			plane;			// 2
	NodePosition	p0;				// 2+2+2 = 6
	NodePosition	p1;				// 2+2+2 = 6
	BYTE			sector;			// 1
	BYTE			light;			// 1
	BYTE			cover[4];		// 4
	BYTE			link_count;		// 1		
} NodeCompressed;					// 2+6+6+1+1+4+1 = 21b + links
#pragma pack()

#endif
