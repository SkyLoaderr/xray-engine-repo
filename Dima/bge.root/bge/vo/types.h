//////////////////////////////////////////////////////////
//                                                      //
//       Project           : Virtual Studio             //
//       SubProject        : Virtual Othello Pro        //
//       Module            : Midgame solver             //
//       Unit              : PVS header                 //
//       File              : types.h                    //
//       Creation date     : 14.02.2001                 //
//       Modification date : 22.01.2002                 //
//       Author            : Dmitriy Iassenev           //
//                                                      //
//////////////////////////////////////////////////////////

#ifndef __VO_TYPES__
#define __VO_TYPES__

//////////////////////////////////////////////////////////
// Macros declarations
//////////////////////////////////////////////////////////

#include <memory.h>

//#define THINK_OPPONENT_TIME

#define WRITE_LOG

#define INFINITY       16384
#ifdef _DEBUG 
#define __ASSERT(bExpression) if (!(bExpression)) {printf("The BUG was noticed here : \n Module : %s \n Line : %d\n",__FILE__,__LINE__);__asm {int 3};}
#else
	#define __ASSERT(bExpression) ;
#endif

#pragma intrinsic(memcpy,memset)

//////////////////////////////////////////////////////////
// Type declarations
//////////////////////////////////////////////////////////

typedef __int64				i64;
typedef unsigned __int64	ui64;
typedef unsigned int		uint;
typedef unsigned short		ui16;
typedef unsigned long		ulong;
typedef unsigned short		ushort;
typedef unsigned char		uchar;
typedef short				TFeature;
typedef char				TBoardCell;

#define ALIGN(alignment)	__declspec(align(alignment))


#endif